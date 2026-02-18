#include "Models/PartialSeason.hpp"
namespace TMDBAPI::Models
{
    PartialSeason::PartialSeason(const Json::Value& json)
    {   
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();
        if (!json.find("name") || json["name"].isNull())
            throw std::invalid_argument("");
        name = json["name"].asString();

        if (!json.find("overview") || json["overview"].isNull())
            throw std::invalid_argument("");
        overview = json["overview"].asString();

        if (!json.find("poster_path"))
            throw std::invalid_argument("");
        if (!json["poster_path"].isNull())
            posterPath = json["poster_path"].asString();

        if (!json.find("air_date"))
            throw std::invalid_argument("");
        if (!json["air_date"].isNull())
            airDate = json["air_date"].asString();

        if (!json.find("season_number"))
            throw std::invalid_argument("");
        if (!json["season_number"].isNull())
            seasonNumber = json["season_number"].asInt();

        if (!json.find("vote_average"))
            throw std::invalid_argument("");
        if (!json["vote_average"].isNull())
            voteAverage = json["vote_average"].asInt();

        //if (!json.find("episode_count"))
        //    throw std::invalid_argument("");
        if (!json["episode_count"].isNull())
            episodeCount = json["episode_count"].asInt();
    }
}