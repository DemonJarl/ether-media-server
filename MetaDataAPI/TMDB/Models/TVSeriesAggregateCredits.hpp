#pragma once
#include "PartialTVSeriesAggregateCredits.hpp"
#include "Models/ModelDefinition.hpp"
namespace TMDBAPI::Models 
{
    struct TVSeriesAggregateCredits : PartialTVSeriesAggregateCredits
    {
        int64_t id;
        ModelClassDeffinitions(TVSeriesAggregateCredits)
    };
}