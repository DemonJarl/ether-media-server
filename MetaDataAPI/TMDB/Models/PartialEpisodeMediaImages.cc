#include "Models/PartialEpisodeMediaImages.hpp"
#include <json/value.h>
#include <vector>
namespace TMDBAPI::Models 
{
    PartialEpisodeMediaImages::PartialEpisodeMediaImages(const Json::Value& json)
    {
        if (!json.find("stills"))
            throw std::invalid_argument("");
        if (!json["stills"].isNull())
        {
            stills.reserve(json["stills"].size());
            for (const Json::Value& backdrop: json["stills"])
                stills.emplace_back(backdrop);
        }
    }
}