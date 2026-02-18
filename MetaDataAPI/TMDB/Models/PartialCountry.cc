#include "Models/PartialCountry.hpp"
namespace TMDBAPI::Models
{
    PartialCountry::PartialCountry(const Json::Value &json)// : BaseEntity(json)
    {
        if (!json.find("iso_3166_1") || json["iso_3166_1"].isNull())
            throw std::invalid_argument("");
        iso3166_1 = json["iso_3166_1"].asString();

        if (!json.find("name") || json["name"].isNull())
            throw std::invalid_argument("");
        name = json["name"].asString();
    }
}