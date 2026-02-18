#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models 
{
    struct MovieExternalIds : Cacheable
    {
        int id{};
        std::string imdbId{};
        std::string wikidataId{};
        std::string facebookId{};
        std::string instagramId{};
        std::string twitterId{};

        ModelClassDeffinitions(MovieExternalIds)
    };

}