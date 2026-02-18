#include "Models/CollectionDetails.hpp"
#include <json/value.h>
namespace TMDBAPI::Models
{
CollectionDetails::CollectionDetails(const Json::Value &json)
{
    if (!json.isMember("id") || json["id"].isNull())
        throw std::invalid_argument("");
    id = json["id"].asInt();

    if (!json.isMember("name"))
        throw std::invalid_argument("");
    name = json["name"].asString();

    if (!json.isMember("original_language"))
        throw std::invalid_argument("");
    if (!json["original_language"].isNull())
        originalLanguage = json["original_language"].asString();

    if (!json.isMember("original_name"))
        throw std::invalid_argument("");
    if (!json["original_name"].isNull())
        originalName = json["original_name"].asString();

    if (!json.isMember("overview"))
        throw std::invalid_argument("");
    if (!json["overview"].isNull())
        overview = json["overview"].asString();

    if (!json.isMember("poster_path"))
        throw std::invalid_argument("");
    if (!json["poster_path"].isNull())
        posterPath = json["poster_path"].asString();

    if (!json.isMember("backdrop_path"))
        throw std::invalid_argument("");
    if (!json["backdrop_path"].isNull())
        backdropPath = json["backdrop_path"].asString();

    if (!json.isMember("parts"))
        throw std::invalid_argument("");
    if (!json["parts"].isNull())
        for (const Json::Value& part : json["parts"])
            parts.emplace_back(part);
}
}