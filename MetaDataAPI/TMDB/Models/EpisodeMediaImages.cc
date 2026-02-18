#include "EpisodeMediaImages.hpp"
#include <json/value.h>
namespace TMDBAPI::Models 
{
    EpisodeMediaImages::EpisodeMediaImages(const Json::Value& json) : PartialEpisodeMediaImages(json)
    {
        if (!json.isMember("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();
    }
}