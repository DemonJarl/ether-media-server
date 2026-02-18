#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models 
{
    struct PartialSeason : Cacheable
    {
        int id{};
        std::string airDate{};
        int episodeCount{0};
        std::string name{};
        std::string overview{};
        std::string posterPath{};
        int seasonNumber{0};
        int voteAverage{0};
        ModelClassDeffinitions(PartialSeason)
    };
}