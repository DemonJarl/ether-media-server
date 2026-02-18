#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models 
{
    struct TVSeriesExternalIds : Cacheable
    {
        int id{};
        std::string freebaseMid{};
        std::string freebaseId{};
        std::string imdbId{};
        int tvrageId{0};
        int tvdbId{0};
        std::string wikidataId{};
        std::string facebookId{};
        std::string instagramId{};
        std::string twitterId{};

        ModelClassDeffinitions(TVSeriesExternalIds)
    };

}