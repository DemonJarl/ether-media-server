#include "Movie.hpp"
#include <json/value.h>

namespace TMDBAPI::Models 
{
    Movie::Movie(const Json::Value& json) : details(json)
    {
        if (json.find("videos") && !json["videos"].isNull())
            videos = std::make_optional(json["videos"]);
        
        if (json.find("images") && !json["images"].isNull())
            images = std::make_optional(json["images"]);
        
        if (json.find("credits") && !json["credits"].isNull())
            credits = std::make_optional(json["credits"]);
        
        if (json.find("external_ids") && !json["external_ids"].isNull())
            images = std::make_optional(json["external_ids"]);
    };
}