#include "Models/TVSeriesExternalIds.hpp"
namespace TMDBAPI::Models 
{
    TVSeriesExternalIds::TVSeriesExternalIds(const Json::Value& json) 
    {
        if (!json.find("id"))
            throw std::invalid_argument("");
        id = json["id"].asInt();

        if (!json.find("imdb_id"))
            throw std::invalid_argument("");
        if (!json["imdb_id"].isNull())
            imdbId = json["imdb_id"].asString();

        if (!json.find("facebook_id"))
            throw std::invalid_argument("");
        if (!json["facebook_id"].isNull())
            facebookId = json["facebook_id"].asString();

        if (!json.find("instagram_id"))
            throw std::invalid_argument("");
        if (!json["instagram_id"].isNull())
            instagramId = json["instagram_id"].asString();

        if (!json.find("tvdb_id"))
            throw std::invalid_argument("");
        if (!json["tvdb_id"].isNull())
            twitterId = json["tvdb_id"].asString();

        if (!json.find("freebase_mid"))
            throw std::invalid_argument("");
        if (!json["freebase_mid"].isNull())
            freebaseMid = json["freebase_mid"].asString();

        if (!json.find("freebase_id"))
            throw std::invalid_argument("");
        if (!json["freebase_id"].isNull())
            freebaseId = json["freebase_id"].asString();

        if (!json.find("wikidata_id"))
            throw std::invalid_argument("");
        if (!json["wikidata_id"].isNull())
            wikidataId = json["wikidata_id"].asString();

        if (!json.find("tvrage_id"))
            throw std::invalid_argument("");
        if (!json["tvrage_id"].isNull())
            tvrageId = json["tvrage_id"].asInt();
    }
}