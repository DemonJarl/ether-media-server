#include "Models/Country.hpp"
namespace TMDBAPI::Models
{
    Country::Country(const Json::Value &json)// : BaseEntity(json)
    {
        if (!json.isMember("iso_3166_1") || json["iso_3166_1"].isNull())
            throw std::invalid_argument("");
        iso3166_1 = json["iso_3166_1"].asString();

        if (!json.isMember("english_name") || json["english_name"].isNull())
            throw std::invalid_argument("");
        englishName = json["english_name"].asString();

        if (!json.isMember("native_name") || json["native_name"].isNull())
            throw std::invalid_argument("");
        nativeName = json["native_name"].asString();
    }
}