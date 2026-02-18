#include "Models/PartialSeasonMediaImages.hpp"
#include <json/value.h>
#include <vector>
namespace TMDBAPI::Models 
{
    PartialSeasonMediaImages::PartialSeasonMediaImages(const Json::Value& json)
    {
        if (!json.find("posters"))
            throw std::invalid_argument("");
        if (!json["posters"].isNull())
        {
            posters.reserve(json["logos"].size());
            for (const Json::Value& poster: json["posters"])
                posters.emplace_back(poster);
        }
    }
}