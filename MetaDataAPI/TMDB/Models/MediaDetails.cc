#include "Models/MediaDetails.hpp"
#include <json/value.h>
#include <trantor/utils/Logger.h>
namespace TMDBAPI::Models
{
MediaDetails::MediaDetails(const Json::Value& json)
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

    if (!json.find("genres"))
        throw std::invalid_argument("");
    if (!json["genres"].isNull())
    {
        genres.reserve(json["genres"].size());
        for (const Json::Value &genre : json["genres"])
            genres.emplace_back(genre);
    }
    
    if (!json.find("homepage"))
        throw std::invalid_argument("");
    if (!json["homepage"].isNull())
        homepage = json["homepage"].asString();

    if (!json.find("overview"))
        throw std::invalid_argument("");
    if (!json["overview"].isNull())
        overview = json["overview"].asString();

    if (!json.find("production_companies"))
        throw std::invalid_argument("");
    if (!json["production_companies"].isNull())
        for (const Json::Value &productionCompany : json["production_companies"])
            productionCompanies.emplace_back(productionCompany);

    if (!json.find("production_countries"))
        throw std::invalid_argument("");
    if (!json["production_countries"].isNull())
        for (const Json::Value &productionCountry : json["production_countries"])
            productionCountries.emplace_back(productionCountry);


    if (!json.find("status"))
        throw std::invalid_argument("");
    status = json["status"].asString();


    if (!json.find("tagline"))
        throw std::invalid_argument("");
    tagline = json["tagline"].asString();

    if (!json.find("original_language"))
        throw std::invalid_argument("");
    originalLanguage = json["original_language"].asString();

    if (!json.find("popularity"))
        throw std::invalid_argument("");
    popularity = json["popularity"].asFloat();

    if (!json.find("spoken_languages"))
        throw std::invalid_argument("");
    if (!json["spoken_languages"].isNull())
        for (const Json::Value &spokenLanguage : json["spoken_languages"])
            spokenLanguages.emplace_back(spokenLanguage);

    if (!json.find("vote_average"))
        throw std::invalid_argument("");
    voteAverage = json["vote_average"].asFloat();

    if (!json.find("vote_count"))
        throw std::invalid_argument("");
    voteCount = json["vote_count"].asInt();
};
}