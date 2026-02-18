#include "SeasonMediaImages.hpp"
#include <json/value.h>
namespace TMDBAPI::Models 
{
    SeasonMediaImages::SeasonMediaImages(const Json::Value& json) : PartialSeasonMediaImages(json)
    {
        if (!json.isMember("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();
    }
}