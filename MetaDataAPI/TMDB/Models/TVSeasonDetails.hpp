#pragma once
#include "Models/EpisodeDetails.hpp"
#include "Models/PartialCompanyNetwork.hpp"
#include "Models/PartialSeason.hpp"
#include <vector>

namespace TMDBAPI::Models
{
    struct TVSeasonDetails : PartialSeason
    {
        std::string _id{}; // ?????
        std::vector<EpisodeDetails> episodes{};
        std::vector<PartialCompanyNetwork> networks{};
        ModelClassDeffinitions(TVSeasonDetails)
    };
}