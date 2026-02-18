#pragma once
#include "AggregateCastCredit.hpp"
#include "AggregateCrewCredit.hpp"
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <vector>

namespace TMDBAPI::Models 
{
    struct PartialTVSeasonAggregateCredits : Cacheable
    {
        std::vector<AggregateCastCredit> cast;
        std::vector<AggregateCastCredit> guestStars;
        std::vector<AggregateCrewCredit> crew;
        ModelClassDeffinitions(PartialTVSeasonAggregateCredits)
    };
}