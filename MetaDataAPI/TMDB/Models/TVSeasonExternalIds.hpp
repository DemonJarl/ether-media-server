#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models 
{
    struct TVSeasonExternalIds : Cacheable
    {
        int id{};
        std::string freebaseMid{};
        std::string freebaseId{};
        int tvrageId{0};
        int tvdbId{0};
        std::string wikidataId{};

        ModelClassDeffinitions(TVSeasonExternalIds)
    };

}