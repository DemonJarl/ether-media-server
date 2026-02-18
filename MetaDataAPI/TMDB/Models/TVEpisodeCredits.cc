#include "TVEpisodeCredits.hpp"
#include "PartialTVEpisodeCredits.hpp"
#include <json/value.h>
#include <stdexcept>
namespace TMDBAPI::Models
{
    TVEpisodeCredits::TVEpisodeCredits(const Json::Value& json) : PartialTVEpisodeCredits(json)
    {
        if (!json.find("id") && json["id"].isNull())
            throw std::invalid_argument("Нету поля id");
        id = json["id"].asInt();
    }
}