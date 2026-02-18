#pragma once
#include "CastCredit.hpp"
#include "CrewCredit.hpp"
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include "PartialCredits.hpp"
#include "PartialTVEpisodeCredits.hpp"
#include <string>
#include <vector>

namespace TMDBAPI::Models 
{
    struct TVEpisodeCredits : PartialTVEpisodeCredits
    {
        int id{};
        ModelClassDeffinitions(TVEpisodeCredits)
    };
}