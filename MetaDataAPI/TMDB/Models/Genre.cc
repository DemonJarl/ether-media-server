#include "Models/Genre.hpp"
#include "json/value.h"
namespace TMDBAPI::Models
{
Genre::Genre(const Json::Value& json)
{
    if (!json.find("id") || json["id"].isNull())
        throw std::invalid_argument("");
    id = json["id"].asInt();
    if (!json.find("name") || json["name"].isNull())
        throw std::invalid_argument("");
    name = json["name"].asString();
}
}