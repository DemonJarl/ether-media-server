#pragma once
#include "BaseEpisode.hpp"
#include "Models/ModelDefinition.hpp"
#include "Models/PartialEpisode.hpp"
#include "Models/CrewCredit.hpp"
#include "Models/CastCredit.hpp"
#include <vector>

namespace TMDBAPI::Models
{
    struct EpisodeDetails : BaseEpisode
    {
        //std::string episodeType{}; // Переделать на enum
        std::vector<CrewCredit> crew{};
        // std::vector<CastCredit> cast{};
        std::vector<CastCredit> guestStars{};
        ModelClassDeffinitions(EpisodeDetails)
    };
}