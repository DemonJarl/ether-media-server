#pragma once

#include "BaseAPI.hpp"
#include <mutex>
#include <shared_mutex>
#include <string>
#include "Utils.h"
#include "DBUtils.h"
#include "coro/sync_wait.hpp"
#include "coro/task.hpp"
#include <filesystem>
#include <system_error>
namespace fs = std::filesystem;

template <typename T>
concept settingValue = std::is_arithmetic_v<T> || std::is_same_v<std::string, T>;

class ServerSettingsManager {
private:
    mutable std::shared_mutex mtx;
    std::string imageSaveDirectory{};
    std::string TMDBAPIKey{};

    template<settingValue T>
    coro::task<bool> getSettingValue(const std::string& settingKey, T& value, const T& defaultValue)
    {
        if (settingKey.empty())
        {
            value = defaultValue;
            co_return false;
        }
        auto res = co_await execSQL(app().getDbClient("serverSettings"), "select value from server_settings where (server_settings.setting_name = $1)", settingKey);
        if (!res)
        {
            value = defaultValue;
            co_return false;
        }
        if ((*res).size() == 0)
        {
            value = defaultValue;
            co_return false;
        }
        value = std::move((*res)[0]["value"].template as<T>());
        co_return true;
    };
    
    template<settingValue T>
    coro::task<bool> setSettingValue(const std::string& settingKey, const T& value)
    {
        co_return (co_await execSQL(app().getDbClient("serverSettings"),"insert into server_settings(setting_name, value) values($1, $2) on conflict do update set value = $2;", settingKey, value)).has_value();
    };
    
    ServerSettingsManager()
    {
        std::string tmp = "../Images";
        getAbsolutePath(tmp);
        
        // если пустой деволтим 
        if (!coro::sync_wait(getSettingValue<std::string>(ServerSetingsKeys::imageSaveLocation, imageSaveDirectory, tmp)) || imageSaveDirectory.empty())
        {
            coro::sync_wait(setSettingValue(ServerSetingsKeys::imageSaveLocation, imageSaveDirectory));
        }
        createDirectoryIfNotExists(imageSaveDirectory);
        coro::sync_wait(getSettingValue<std::string>(ServerSetingsKeys::TMDBAPIKey, TMDBAPIKey, ""));
    }

public:

    ServerSettingsManager(ServerSettingsManager const&) = delete;
    void operator=(ServerSettingsManager const&)  = delete;

    struct ServerSetingsKeys
    {
        static const std::string imageSaveLocation;
        static const std::string TMDBAPIKey;
    };
    
    std::string getTMDBKey() const
    {
        std::shared_lock lock(mtx);
        return TMDBAPIKey;
    }

    void setTMDBKey(const std::string& key)
    {
        std::unique_lock lock(mtx);

        if (!coro::sync_wait(setSettingValue(ServerSetingsKeys::imageSaveLocation, key)))
            return;
        // lock.lock();
        TMDBAPIKey = key;
        TMDBAPI::BaseAPI::setKey(key);
    }


    std::string getImageSaveDirectory() const
    {
        std::shared_lock lock(mtx);
        return imageSaveDirectory;
    }

    void setImageSaveDirectory(const std::string& directory) 
    {
        if (directory.empty())
            return;
        if (!pathExists(directory))
            return;

        if (!coro::sync_wait(setSettingValue(ServerSetingsKeys::TMDBAPIKey, directory)))
            return;
        std::unique_lock lock(mtx);
        imageSaveDirectory = directory;
    }

    static ServerSettingsManager& Instance()
    {
        static ServerSettingsManager instance;
        return instance;
    }

};