#include "Models/PersonExternalIds.hpp"
namespace TMDBAPI::Models 
{
    PersonExternalIds::PersonExternalIds(const Json::Value& json)
    {
        if (!json.find("id") || json["id"].isNull())
            //throw std::invalid_argument("");
        {
            
        }
        else
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

        if (!json.find("tiktok_id"))
            throw std::invalid_argument("");
        if (!json["tiktok_id"].isNull())
            tiktokId = json["tiktok_id"].asString();

        if (!json.find("twitter_id"))
            throw std::invalid_argument("");
        if (!json["twitter_id"].isNull())
            twitterId = json["twitter_id"].asString();

        if (!json.find("youtube_id"))
            throw std::invalid_argument("");
        if (!json["youtube_id"].isNull())
            youtubeId = json["youtube_id"].asString();

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