#include "Models/Language.hpp"
namespace TMDBAPI::Models
{
Language::Language(const Json::Value &json)
{
    if (!json.isMember("iso_639_1") || json["iso_639_1"].isNull())
        throw std::invalid_argument("");
    iso639_1 = json["iso_639_1"].asString();

    if (!json.isMember("english_name") || json["english_name"].isNull())
        throw std::invalid_argument("");
    englishName = json["english_name"].asString(); 

    if (!json.isMember("name") || json["name"].isNull())
        throw std::invalid_argument("");
    name = json["name"].asString(); 
}
}