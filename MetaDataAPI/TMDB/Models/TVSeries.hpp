#pragma once
#include "AggregateCastCredit.hpp"
#include "CastCredit.hpp"
#include "Models/Cacheable.hpp"
#include "Models/MediaImages.hpp"
#include "Models/TVSeriesDetails.hpp"
#include "Models/Videos.hpp"
#include "PartialCredits.hpp"
#include "PartialMediaImages.hpp"
#include "PartialMovieDetails.hpp"
#include "PartialTVSeriesAggregateCredits.hpp"
#include "TVSeriesAggregateCredits.hpp"
#include "Credits.hpp"
#include "TVSeriesExternalIds.hpp"
#include <json/value.h>
#include <optional>
#include <vector>
namespace TMDBAPI::Models
{
    class TVSeries : public Cacheable
    {
        int id{};
    public:
        TVSeriesDetails details;
        std::optional<PartialMediaImages> images{};
        std::optional<Videos> videos;
        std::optional<TVSeriesExternalIds> externalIDs;
        std::optional<PartialCredits> credits;
        std::optional<PartialTVSeriesAggregateCredits> aggregateCredits;
        ModelClassDeffinitions(TVSeries)
    };
}