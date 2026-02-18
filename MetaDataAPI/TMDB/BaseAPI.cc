#include "BaseAPI.hpp"
#include "Enums/Enums.hpp"
#include <expected>
#include <format>
#include <iostream>
#include <json/config.h>
#include <json/reader.h>
#include "ICacheEngine.hpp"
#include "Models/PartialMovieDetails.hpp"
#include "Models/PartialTVDetails.hpp"
#include "Models/SearchPersonDetails.hpp"
#include "cpr/cpr.h"
#include "cpr/curl_container.h"
#include "cpr/parameters.h"
#include <json/value.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <trantor/utils/Logger.h>

namespace TMDBAPI {

Languages BaseAPI::language = Languages::Ru;
std::string BaseAPI::key;
std::unique_ptr<ICacheEngine> BaseAPI::cacheEngine;
// BaseAPI::BaseAPI(const std::string key, const std::string language, const std::string region)
// {
//     if (key.empty())
//             throw std::invalid_argument("TMDB API key cannot be empty");
//     setKey(key);
//     if (key.empty())
//         throw std::invalid_argument("Language cannot be empty");
//     setLanguage(language);
//     if (key.empty())
//         throw std::invalid_argument("Region cannot be empty");
//     this->region = region;
// }

// BaseAPI::BaseAPI(const std::string key) : BaseAPI(key, "ru-RU", "Russia"){}

std::expected<Json::Value, APIError> BaseAPI::get(std::string url)
{
    std::cout << "URL: " << url << std::endl;
    Json::Value ret;
    Json::Reader reader; 
    cpr::Response resp = cpr::Get(cpr::Url(url), cpr::Header{{"Authorization","Bearer "+BaseAPI::key}});
    //std::cout<<"Get status_code: "<<resp.status_code<<std::endl;
    //std::cout<<"JSON: "<<resp.text<<std::endl;
    if (resp.error.code != cpr::ErrorCode::OK)
    {
        std::cout<<"Error code: "<<std::to_string(resp.error.code )<<std::endl;
        return std::unexpected(APIError::TransportError);
    }
    if (resp.status_code != 200)
    {
        if (reader.parse(resp.text, ret))
        {
            if (ret.find("status_code") && !ret["status_code"].isNull())
            {
                return std::unexpected(static_cast<APIError>(ret["status_code"].asInt()));
            }
            else 
            {
                return std::unexpected(APIError::TransportError);
            }
        }
        else 
        {
            LOG_ERROR << "URL: " << url;
            LOG_ERROR<<std::format("json with error: {}", resp.text);
            return std::unexpected(APIError::JSONParsingError);
        }
        //return std::unexpected();
    }
    if (!reader.parse(resp.text, ret))
    {
        LOG_ERROR << "URL: " << url;
        LOG_ERROR<<std::format("json with error: {}", resp.text);
        return std::unexpected(APIError::JSONParsingError);
    }
    return ret;
}

// template<>
// std::expected<std::variant<Models::SearchPersonDetails, Models::PartialTVDetails, Models::PartialMovieDetails>, APIError> BaseAPI::getModel(const std::string& endpoint)
// {
//     std::optional<Json::Value> cache = cacheEngine->getCache(endpoint);
//     if (cache.has_value())
//     {
//         // кэш всегда валидный
//         MediaType type = strToMediaType(cache.value()["media_type"].asString());
//         switch (type) 
//         {
//             case MediaType::TV:
//                 return  Models::PartialTVDetails{cache.value()};
//                 break;
//             case MediaType::Person:
//                 return  Models::SearchPersonDetails{cache.value()};
//                 break;
//             case MediaType::Movie:
//                 return  Models::PartialMovieDetails{cache.value()};
//                 break;
//         }
//         return std::unexpected(APIError::TransportError);
//     }
//     std::expected<Json::Value, APIError> json = get(endpoint);
//     if (json.has_value())
//     {
//         try 
//         {
//             cacheEngine->putCache(endpoint, json.value());
//             MediaType type = strToMediaType(json.value()["media_type"].asString());
//             switch (type) 
//             {
//                 case MediaType::TV:
//                     return  Models::PartialTVDetails{json.value()};
//                     break;
//                 case MediaType::Person:
//                     return  Models::SearchPersonDetails{json.value()};
//                     break;
//                 case MediaType::Movie:
//                     return  Models::PartialMovieDetails{json.value()};
//                     break;
//             }
//         } 
//         catch (std::invalid_argument e) 
//         {
//             return std::unexpected(APIError::JSONParsingError);
//         }
//     }
//     else 
//     {
//         return std::unexpected(json.error());
//     }
//     return std::unexpected(APIError::TransportError);
// }

}