#include "Models/EpisodeDetails.hpp"
#include "BaseEpisode.hpp"
#include <json/value.h>
#include <stdexcept>
namespace TMDBAPI::Models
{
    EpisodeDetails::EpisodeDetails(const Json::Value &json) : BaseEpisode(json)
    {
        if (!json.find("guest_stars"))
            throw std::invalid_argument("");
        if (!json["guest_stars"].isNull())
        {
            const Json::Value& arr = json["guest_stars"];
            guestStars.reserve(arr.size());
            for (const Json::Value& star : arr)
                guestStars.emplace_back(star);
        }
        if (!json.find("crew"))
            throw std::invalid_argument("");
        if (!json["crew"].isNull())
        {
            const Json::Value& arr = json["crew"];
            crew.reserve(arr.size());
            for (const Json::Value& crewMember : arr)
                crew.emplace_back(crewMember);
        }
    }
}