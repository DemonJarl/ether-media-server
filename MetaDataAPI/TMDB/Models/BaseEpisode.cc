#include "BaseEpisode.hpp"
namespace TMDBAPI::Models 
{
    BaseEpisode::BaseEpisode(const Json::Value& json)
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();
        if (!json.find("name") || json["name"].isNull())
            throw std::invalid_argument("");
        name = json["name"].asString();

        if (!json.find("overview"))
            throw std::invalid_argument("");
        if (!json["overview"].isNull())
            overview = json["overview"].asString();

        if (!json.find("vote_average"))
            throw std::invalid_argument("");
        if (!json["vote_average"].isNull())
            voteAverage = json["vote_average"].asInt();

        if (!json.find("vote_count"))
            throw std::invalid_argument("");
        if (!json["vote_count"].isNull())
            voteCount = json["vote_count"].asInt();

        if (!json.find("air_date"))
            throw std::invalid_argument("");
        if (!json["air_date"].isNull())
            airDate = json["air_date"].asString();

        if (!json.find("episode_number"))
            throw std::invalid_argument("");
        if (!json["episode_number"].isNull())
            episodeNumber = json["episode_number"].asInt();

        if (!json.find("runtime"))
            throw std::invalid_argument("");
        if (!json["runtime"].isNull())
            runtime = json["runtime"].asInt();

        if (!json.find("season_number"))
            throw std::invalid_argument("");
        if (!json["season_number"].isNull())
            seasonNumber = json["season_number"].asInt();
        
        if (!json.find("still_path"))
            throw std::invalid_argument("");
        if (!json["still_path"].isNull())
            stillPath = json["still_path"].asString();

    }
}