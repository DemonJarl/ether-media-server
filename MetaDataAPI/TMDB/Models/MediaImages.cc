#include "Models/MediaImages.hpp"
#include "PartialMediaImages.hpp"
#include <json/value.h>
#include <vector>
namespace TMDBAPI::Models 
{
    MediaImages::MediaImages(const Json::Value& json) : PartialMediaImages(json)
    {
        if (!json.isMember("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();
    }
}