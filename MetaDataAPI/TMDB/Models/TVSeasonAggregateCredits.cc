#include "TVSeasonAggregateCredits.hpp"
#include <json/value.h>
#include <stdexcept>
namespace TMDBAPI::Models
{
    TVSeasonAggregateCredits::TVSeasonAggregateCredits(const Json::Value& json) : PartialTVSeasonAggregateCredits(json)
    {
        if (!json.find("id") && json["id"].isNull())
            throw std::invalid_argument("Нету поля id");
    }
}