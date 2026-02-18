#include "TVSeriesAggregateCredits.hpp"
#include <json/value.h>
#include <stdexcept>
namespace TMDBAPI::Models
{
    TVSeriesAggregateCredits::TVSeriesAggregateCredits(const Json::Value& json) : PartialTVSeriesAggregateCredits(json)
    {
        if (!json.find("id") && json["id"].isNull())
            throw std::invalid_argument("Нету поля id");
        id = json["id"].asInt();
    }
}