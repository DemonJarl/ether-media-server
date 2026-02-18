#pragma once
#include "Models/PartialTVEpisodeExternalIds.hpp"
#include "Models/ModelDefinition.hpp"

namespace TMDBAPI::Models 
{
    struct TVEpisodeExternalIds : PartialTVEpisodeExternalIds
    {
        int id{};

        ModelClassDeffinitions(TVEpisodeExternalIds)
    };

}