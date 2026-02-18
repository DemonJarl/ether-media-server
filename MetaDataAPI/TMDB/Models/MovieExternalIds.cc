#include "Models/MovieExternalIds.hpp"
namespace TMDBAPI::Models 
{
    MovieExternalIds::MovieExternalIds(const Json::Value& json)
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();

        if (!json.find("imdb_id"))
            throw std::invalid_argument("");
        if (!json["imdb_id"].isNull())
            imdbId = json["imdb_id"].asString();

        if (!json.find("wikidata_id"))
            throw std::invalid_argument("");
        if (!json["wikidata_id"].isNull())
            wikidataId = json["wikidata_id"].asString();

        if (!json.find("facebook_id"))
            throw std::invalid_argument("");
        if (!json["facebook_id"].isNull())
            facebookId = json["facebook_id"].asString();

        if (!json.find("instagram_id"))
            throw std::invalid_argument("");
        if (!json["instagram_id"].isNull())
            instagramId = json["instagram_id"].asString();

        if (!json.find("twitter_id"))
            throw std::invalid_argument("");
        if (!json["twitter_id"].isNull())
            twitterId = json["twitter_id"].asString();
    }
}