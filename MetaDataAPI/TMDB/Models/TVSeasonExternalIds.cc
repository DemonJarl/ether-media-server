#include "Models/TVSeasonExternalIds.hpp"
namespace TMDBAPI::Models 
{
    TVSeasonExternalIds::TVSeasonExternalIds(const Json::Value& json) 
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();

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

        if (!json.find("tvdb_id"))
            throw std::invalid_argument("");
        if (!json["tvdb_id"].isNull())
            tvdbId = json["tvdb_id"].asInt();
    }
}