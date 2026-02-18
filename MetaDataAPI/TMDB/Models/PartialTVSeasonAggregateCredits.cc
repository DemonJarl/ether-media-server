#include "PartialTVSeasonAggregateCredits.hpp"
#include <json/value.h>
#include <stdexcept>
namespace TMDBAPI::Models
{
    PartialTVSeasonAggregateCredits::PartialTVSeasonAggregateCredits(const Json::Value& json)
    {
        if(json.find("cast") && !json["cast"].isNull())
        {
            const Json::Value& arr = json["cast"];
            cast.reserve(arr.size());
            for(const Json::Value& c : arr)
                cast.emplace_back(c);
        }
        else 
            throw std::invalid_argument("Нету поля cast");

        if(json.find("guest_stars") && !json["guest_stars"].isNull())
        {
            const Json::Value& arr = json["guest_stars"];
            guestStars.reserve(arr.size());
            for(const Json::Value& c : arr)
                guestStars.emplace_back(c);
        }
        else 
            throw std::invalid_argument("Нету поля guest_stars");

        
        if(json.find("crew") && !json["crew"].isNull())
        {
            const Json::Value& arr = json["crew"];
            crew.reserve(arr.size());
            for(const Json::Value& c : arr)
                crew.emplace_back(c);
        }
        else 
            throw std::invalid_argument("Нету поля crew");
    }
}