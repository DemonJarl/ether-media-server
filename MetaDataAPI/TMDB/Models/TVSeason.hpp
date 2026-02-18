#pragma once
#include "Models/Cacheable.hpp"
#include "Models/MediaImages.hpp"
#include "Models/ModelDefinition.hpp"
#include "Models/TVSeasonDetails.hpp"
#include "Models/Videos.hpp"
#include "PartialCredits.hpp"
#include "PartialMediaImages.hpp"
#include "PartialSeasonMediaImages.hpp"
#include "PartialTVSeasonAggregateCredits.hpp"
#include "TVSeasonAggregateCredits.hpp"
#include "TVSeasonExternalIds.hpp"

namespace TMDBAPI::Models 
{
    struct TVSeason : Cacheable
    {
        TVSeasonDetails details;
        std::optional<PartialSeasonMediaImages> images{};
        std::optional<Videos> videos;
        std::optional<TVSeasonExternalIds> externalIDs;
        std::optional<PartialTVSeasonAggregateCredits> aggregateCredits;
        std::optional<PartialCredits> credits;
        ModelClassDeffinitions(TVSeason)
    };
}