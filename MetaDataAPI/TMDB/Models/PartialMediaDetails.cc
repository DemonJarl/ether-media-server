#include "Models/PartialMediaDetails.hpp"
#include "Enums/Enums.hpp"
#include <iostream>
#include <json/value.h>
#include <string>
namespace TMDBAPI::Models 
{
    PartialMediaDetails::PartialMediaDetails(const Json::Value& json)
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();

        if (!json.find("adult") || json["adult"].isNull())
            throw std::invalid_argument("");
        adult = json["adult"].asBool();

        if (!json.find("backdrop_path"))
            throw std::invalid_argument("");
        if (!json["backdrop_path"].isNull())
            backdropPath = json["backdrop_path"].asString();

        if (!json.find("poster_path"))
            throw std::invalid_argument("");
        if (!json["poster_path"].isNull())
            posterPath = json["poster_path"].asString();

        if (!json.find("genre_ids"))
            throw std::invalid_argument("");
        if (!json["genre_ids"].isNull())
            for (const Json::Value &genre : json["genre_ids"])
                genreIds.push_back(genre.asInt());

        if (!json.find("overview"))
            throw std::invalid_argument("");
        if (!json["overview"].isNull())
            overview = json["overview"].asString();

        if (!json.find("original_language"))
            throw std::invalid_argument("");
        originalLanguage = json["original_language"].asString();

        if (!json.find("popularity"))
            throw std::invalid_argument("");
        popularity = json["popularity"].asFloat();

        if (!json.find("vote_average"))
            throw std::invalid_argument("");
        voteAverage = json["vote_average"].asFloat();

        if (!json.find("media_type"))
            throw std::invalid_argument("");
        std::string type = json["media_type"].asString();
        if (type == "movie")
            mediaType = MediaType::Movie;
        if (type == "tv")
            mediaType = MediaType::TV;
        if (!json.find("name"))
            throw std::invalid_argument("");
        name = json["name"].asString();

        if (json.find("original_name"))
            originalTitle = json["original_name"].asString();
        if (json.find("original_title"))
            originalTitle = json["original_title"].asString();
        if (originalTitle.empty())
            throw std::invalid_argument("");

        if (!json.find("original_language"))
            throw std::invalid_argument("");
        originalLanguage = json["original_language"].asString();

        if (!json.find("popularity"))
            throw std::invalid_argument("");
        popularity = json["popularity"].asFloat();

        if (!json.find("vote_average"))
            throw std::invalid_argument("");
        voteAverage = json["vote_average"].asFloat();

        if (!json.find("vote_count"))
            throw std::invalid_argument("");
        voteCount = json["vote_count"].asInt();
    }
}