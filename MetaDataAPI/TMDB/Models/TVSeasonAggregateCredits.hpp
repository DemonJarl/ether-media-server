#pragma once
#include "PartialTVSeasonAggregateCredits.hpp"
#include "Models/ModelDefinition.hpp"

namespace TMDBAPI::Models 
{
    struct TVSeasonAggregateCredits : PartialTVSeasonAggregateCredits
    {
        int64_t id;
        ModelClassDeffinitions(TVSeasonAggregateCredits)
    };
}