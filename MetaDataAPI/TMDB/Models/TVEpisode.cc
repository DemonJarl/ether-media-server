#include "Models/TVEpisode.hpp"
#include <json/value.h>

namespace TMDBAPI::Models 
{
    TVEpisode::TVEpisode(const Json::Value& json) : details(json)
    {
        if (json.find("images") && !json["images"].isNull())
            images = std::make_optional(json["images"]);
        
        if (json.find("external_ids") && !json["external_ids"].isNull())
            externalIDs = std::make_optional(json["external_ids"]);
        
        if (json.find("credits") && !json["credits"].isNull())
            credits = std::make_optional(json["credits"]);
    };
}