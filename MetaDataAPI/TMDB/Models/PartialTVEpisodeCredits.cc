#include "PartialTVEpisodeCredits.hpp"
#include "PartialCredits.hpp"
#include <json/value.h>
#include <stdexcept>
namespace TMDBAPI::Models
{
    PartialTVEpisodeCredits::PartialTVEpisodeCredits(const Json::Value& json) : PartialCredits(json)
    {
        if(json.find("guest_stars") && !json["guest_stars"].isNull())
        {
            const Json::Value& arr = json["guest_stars"];
            guestStars.reserve(arr.size());
            for(const Json::Value& c : arr)
                guestStars.emplace_back(c);
        }
        else 
            throw std::invalid_argument("Нету поля guest_stars");
    }
}