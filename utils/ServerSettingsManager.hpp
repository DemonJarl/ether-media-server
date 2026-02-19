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
    coro::task<T> getSettingValue(const std::string& settingKey)
    {
        if (settingKey.empty())
            co_return {};
        auto res = co_await execSQL(app().getDbClient("serverSettings"), "select value from server_settings where (server_settings.setting_name = $1)", settingKey);
        if (!res.has_value())
            co_return {};
        if ((*res).size() == 0)
            co_return {};
        co_return (*res)[0]["value"].template as<T>();
    };

    template<settingValue T>
    coro::task<bool> setSettingValue(const std::string& settingKey, const T& value)
    {
        co_return (co_await execSQL(app().getDbClient("serverSettings"),"insert into server_settings(setting_name, value) values($1, $2) on conflict do update set value = $2;", settingKey, value)).has_value();
    };
    
    ServerSettingsManager()
    {
        imageSaveDirectory = coro::sync_wait(getSettingValue<std::string>(ServerSetingsKeys::imageSaveLocation));
        // если пустой деволтим 
        if (imageSaveDirectory.empty())
        {
            imageSaveDirectory = "../Images";
            coro::sync_wait(setSettingValue(ServerSetingsKeys::imageSaveLocation, imageSaveDirectory));
        }
        createDirectoryIfNotExists(imageSaveDirectory);
        TMDBAPIKey = coro::sync_wait(getSettingValue<std::string>(ServerSetingsKeys::TMDBAPIKey));
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