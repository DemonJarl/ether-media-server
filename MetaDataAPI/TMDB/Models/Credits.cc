#include "Credits.hpp"
#include <json/value.h>
#include <stdexcept>
namespace TMDBAPI::Models
{
    Credits::Credits(const Json::Value& json) : PartialCredits(json)
    {
        if (!json.find("id") && json["id"].isNull())
            throw std::invalid_argument("Нету поля id");
    }
}