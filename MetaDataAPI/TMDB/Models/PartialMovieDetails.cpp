#include "Models/PartialMovieDetails.hpp"
#include "Enums/Enums.hpp"
#include <iostream>
#include <json/value.h>
#include <stdexcept>
#include <string>
namespace TMDBAPI::Models 
{
    PartialMovieDetails::PartialMovieDetails(const Json::Value& json)
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();

        if (!json.find("adult") || json["adult"].isNull())
            throw std::invalid_argument("");
        adult = json["adult"].asBool();

        if (!json.find("video") || json["video"].isNull())
            throw std::invalid_argument("");
        adult = json["video"].asBool();

        if (!json.find("backdrop_path"))
            throw std::invalid_argument("");
        if (!json["backdrop_path"].isNull())
            backdropPath = json["backdrop_path"].asString();

        if (!json.isMember("title"))
            throw std::invalid_argument("");
        title = json["title"].asString();

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

        if (!json.find("vote_average"))
            throw std::invalid_argument("");
        voteAverage = json["vote_average"].asFloat();

        if (!json.find("original_title"))
            throw std::invalid_argument("");
        originalTitle = json["original_title"].asString();

        if (!json.find("original_language"))
            throw std::invalid_argument("");
        originalLanguage = json["original_language"].asString();

        if (!json.find("popularity"))
            throw std::invalid_argument("");
        popularity = json["popularity"].asFloat();
        
        if (!json.find("release_date"))
            throw std::invalid_argument("");
        releaseDate = json["release_date"].asString();

        if (!json.find("vote_count"))
            throw std::invalid_argument("");
        voteCount = json["vote_count"].asInt();
    }
}