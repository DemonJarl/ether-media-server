#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models
{
    struct BaseEpisode : Cacheable
    {
        int id{};
        std::string name{};
        std::string overview{};
        float voteAverage{};
        int voteCount{};
        std::string airDate{};
        int episodeNumber{};
        std::string productionCode{};
        int runtime{0};
        int seasonNumber{0};
        std::string stillPath{};
        ModelClassDeffinitions(BaseEpisode)
    };
}