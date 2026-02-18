#include "Models/SeasonImages.hpp"
namespace TMDBAPI::Models 
{
    SeasonImages::SeasonImages(const Json::Value& json)
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();
        if (!json.find("posters"))
            throw std::invalid_argument("");
        if (!json["posters"].isNull())
            for (const Json::Value& poster: json["posters"])
                posters.emplace_back(poster);
    }
}