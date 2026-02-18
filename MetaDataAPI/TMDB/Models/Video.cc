#include "Models/Video.hpp"
#include <stdexcept>

namespace TMDBAPI::Models 
{
    Video::Video(const Json::Value& json)
    {
        if (!json.find("iso_639_1"))
            throw std::invalid_argument("");
        if (!json["iso_639_1"].isNull())
            iso639_1 = json["iso_639_1"].asString();

        if (!json.find("iso_3166_1"))
            throw std::invalid_argument("");
        if (!json["iso_3166_1"].isNull())
            iso3166_1 = json["iso_3166_1"].asString();

        if (!json.find("name"))
            throw std::invalid_argument("");
        if (!json["name"].isNull())
            name = json["name"].asString();

        if (!json.find("key"))
            throw std::invalid_argument("");
        if (!json["key"].isNull())
            key = json["key"].asString();

        if (!json.find("site"))
            throw std::invalid_argument("");
        if (!json["site"].isNull())
            site = json["site"].asString();

        if (!json.find("size"))
            throw std::invalid_argument("");
        if (!json["size"].isNull())
            size = json["size"].asInt();

        if (!json.find("type"))
            throw std::invalid_argument("");
        if (!json["type"].isNull())
            type = json["type"].asString();

        if (!json.find("official"))
            throw std::invalid_argument("");
        if (!json["official"].isNull())
            official = json["official"].asBool();

        if (!json.find("published_at"))
            throw std::invalid_argument("");
        if (!json["published_at"].isNull())
            publishedAt = json["published_at"].asString();

        if (!json.find("id"))
            throw std::invalid_argument("");
        if (!json["id"].isNull())
            id = json["id"].asString();
    }
}