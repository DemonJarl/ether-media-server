#pragma once
#include <expected>
#include "Enums/Enums.hpp"
#include "ICacheEngine.hpp"
#include <cstdint>
#include <iostream>
#include <json/value.h>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <trantor/utils/Logger.h>

namespace TMDBAPI {

const std::string BaseURL = "https://api.themoviedb.org";
const std::string baseImageURL = "http://image.tmdb.org/t/p";
const std::string secureImageURL = "https://image.tmdb.org/t/p";

class BaseAPI
{
protected:
    static const uint8_t version = 3;
    static std::unique_ptr<ICacheEngine> cacheEngine;
    static std::string key;
    static Languages language;
    std::string baseURL = "https://api.themoviedb.org";
    static std::string region;
    //friend void setKey(const std::string&key);
    //friend void setLanguage(const Languages);
    //friend void setCache(std::unique_ptr<ICacheEngine> cache);
    BaseAPI() = default;
//private:

public:
    static Languages getLanguage(){
        return language;
    }

    static std::string getRegion(){
        return region;
    }

    static std::string getKey(){
        return key;
    }

    const uint8_t getVersion(){
        return version;
    }
    static void clearCache()
    {
        cacheEngine->clearCache();
    }
    static void setRegion(const std::string& region)
    {
        // if (region.empty())
        //     return;
        BaseAPI::region = region;
    }

    static void setKey(const std::string& key)
    {
        // if (key.empty())
        //     return;
        BaseAPI::key = key;
    }

    static void setLanguage(const Languages language)
    {
        BaseAPI::language = language;
    }

    static void setCache(std::unique_ptr<ICacheEngine> cache)
    {
        // if (!cache)
        //     return;
        BaseAPI::cacheEngine = std::move(cache);
    }
    
protected:
    std::optional<Json::Value> getCache(const std::string& url)
    {
        if (!cacheEngine)
            return {};
        return cacheEngine->getCache(url);
    }
    void putCache(const std::string& url, const Json::Value& json)
    {
        if (!cacheEngine)
            return;
        cacheEngine->putCache(url, json);
    }
    template<class Model>
    std::expected<Model, APIError> getModel(const std::string& endpoint)
    {
        std::cout<<"Endpoint: "<<endpoint<<std::endl;
        std::optional<Json::Value> cache = getCache(endpoint);
        if (cache.has_value())
        {
            std::cout<<"Found Cache "<<std::endl;
            return {cache.value()};
        }
        std::expected<Json::Value, APIError> json = get(endpoint);
        if (json.has_value())
        {
            // LOG_DEBUG<<std::format("Endpoint: {}\n json {} ", endpoint, json->toStyledString());
            try
            {
                putCache(endpoint, *json);
                return {*json};
            }
            catch (std::invalid_argument e) 
            {
                LOG_ERROR<<std::format("URL {}", endpoint);
                LOG_ERROR<<std::format("json with errors: {}", json->toStyledString());
                return std::unexpected(APIError::JSONParsingError);
            }
        }
        else 
        {
            return std::unexpected(json.error());
        }
    }
    
    template<class Model>
    std::expected<Model, APIError> getPagedResult(const std::string& endpoint)
    {
        std::optional<Json::Value> cache = getCache(endpoint);
        if (cache.has_value())
        {
            std::cout<<"Found Cache "<<std::endl;
            return Model{cache.value(), endpoint};
        }
        std::expected<Json::Value, APIError> json = get(endpoint);
        if (json.has_value())
        {
            LOG_ERROR<<std::format("json: {}", json->toStyledString());
            try 
            {
                putCache(endpoint, json.value());
                return Model{json.value(), endpoint};
            } 
            catch (std::invalid_argument e) 
            {
                LOG_ERROR<<std::format("URL {}", endpoint);
                LOG_ERROR<<std::format("json with errors: {}", json->toStyledString());
                return std::unexpected(APIError::JSONParsingError);
            }
        }
        else 
        {
            return std::unexpected(json.error());
        }
    }
    
    std::expected<Json::Value, APIError> get(std::string url);
};

// inline void setKey(const std::string& key)
// {
//     if (key.empty())
//         return;
//     BaseAPI::key = key;
// }

// inline void setLanguage(const Languages language)
// {
//     BaseAPI::language = language;
// }

// inline void setCache(std::unique_ptr<ICacheEngine> cache)
// {
//     if (!cache)
//         return;
//     BaseAPI::cacheEngine = std::move(cache);
// }
}