#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models 
{
    struct PersonExternalIds : Cacheable
    {
        int id{};
        std::string freebaseMid{};
        std::string freebaseId{};
        std::string imdbId{};
        int tvrageId{0};
        std::string wikidataId{};
        std::string facebookId{};
        std::string instagramId{};
        std::string tiktokId{};
        std::string twitterId{};
        std::string youtubeId{};

        ModelClassDeffinitions(PersonExternalIds)
    };

}