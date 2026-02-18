#pragma once
#include "AggregateCastCredit.hpp"
#include "AggregateCrewCredit.hpp"
#include "CastCredit.hpp"
#include "CrewCredit.hpp"
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>
#include <vector>

namespace TMDBAPI::Models 
{
    struct PartialTVSeriesAggregateCredits : Cacheable
    {
        std::vector<AggregateCastCredit> cast;
        std::vector<AggregateCastCredit> guestStars;
        std::vector<AggregateCrewCredit> crew;
        ModelClassDeffinitions(PartialTVSeriesAggregateCredits)
    };
}