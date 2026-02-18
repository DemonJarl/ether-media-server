#pragma once
#include "EpisodeDetails.hpp"
#include "Models/Cacheable.hpp"
#include "Models/PartialTVEpisodeExternalIds.hpp"
#include "Models/ModelDefinition.hpp"
#include "PartialCredits.hpp"
#include "PartialEpisodeMediaImages.hpp"
#include "PartialMediaImages.hpp"
#include "PartialTVEpisodeCredits.hpp"

namespace TMDBAPI::Models 
{
    struct TVEpisode : Cacheable
    {
        EpisodeDetails details;
        std::optional<PartialTVEpisodeExternalIds> externalIDs;
        std::optional<PartialTVEpisodeCredits> credits;
        std::optional<PartialEpisodeMediaImages> images;
        ModelClassDeffinitions(TVEpisode)
    };
}