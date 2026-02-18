#include "Models/TVSeasonDetails.hpp"
#include "Models/PartialSeason.hpp"
#include <iostream>
#include <json/value.h>
#include <stdexcept>

namespace TMDBAPI::Models
{
    TVSeasonDetails::TVSeasonDetails(const Json::Value& json) : PartialSeason(json)
    {
        
        if(!json.find("_id") || json["_id"].isNull())
            throw std::invalid_argument("");
        _id = json["_id"].asString();

        if(!json.find("episodes") || !json["episodes"].isNull())
        {
            const Json::Value& arr = json["episodes"];
            episodes.reserve(arr.size());
            for(const Json::Value& episode : arr)
                episodes.emplace_back(episode);
        }
        else 
            throw std::invalid_argument("");

        if(!json.find("networks") || !json["networks"].isNull())
        {
            const Json::Value& arr = json["networks"];
            networks.reserve(arr.size());
            for(const Json::Value& network : arr)
                networks.emplace_back(network);
        }
        else 
            throw std::invalid_argument("");
    }
}