#pragma once
#include "Models/BaseEpisode.hpp"
#include "Models/ModelDefinition.hpp"

namespace TMDBAPI::Models
{
    struct PartialEpisode : BaseEpisode
    {
        int showId{0};
        ModelClassDeffinitions(PartialEpisode)
    };
}