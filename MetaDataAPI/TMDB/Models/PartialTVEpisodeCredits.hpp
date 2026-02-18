#pragma once
#include "CastCredit.hpp"
#include "Models/ModelDefinition.hpp"
#include "PartialCredits.hpp"
#include <vector>

namespace TMDBAPI::Models 
{
    struct PartialTVEpisodeCredits : PartialCredits
    {
        std::vector<CastCredit> guestStars;
        ModelClassDeffinitions(PartialTVEpisodeCredits)
    };
}