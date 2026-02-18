#include "Models/PartialMediaImages.hpp"
#include <json/value.h>
#include <vector>
namespace TMDBAPI::Models 
{
    PartialMediaImages::PartialMediaImages(const Json::Value& json)
    {
        if (!json["backdrops"].isNull())
        {
            backdrops.reserve(json["logos"].size());
            for (const Json::Value& backdrop: json["backdrops"])
                backdrops.emplace_back(backdrop);
        }
        
        if (!json.find("logos"))
            throw std::invalid_argument("");
        if (!json["logos"].isNull())
        {
            logos.reserve(json["logos"].size());
            for (const Json::Value& logo: json["logos"])
                logos.emplace_back(logo);
        }

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