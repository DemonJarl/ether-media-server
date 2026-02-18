#pragma once
#include "MediaImages.hpp"
#include "ModelDefinition.hpp"
#include "Models/Cacheable.hpp"
#include "Models/Credits.hpp"
#include "MovieCastCredit.hpp"
#include "MovieDetails.hpp"
#include "MovieExternalIds.hpp"
#include "PartialCredits.hpp"
#include "TVSeriesAggregateCredits.hpp"
#include "Videos.hpp"
namespace TMDBAPI::Models
{
struct Movie : Cacheable
{
    MovieDetails details;
    std::optional<PartialMediaImages> images{};
    std::optional<Videos> videos;
    std::optional<MovieExternalIds> externalIDs;
    std::optional<PartialCredits> credits;
    ModelClassDeffinitions(Movie);
};
}