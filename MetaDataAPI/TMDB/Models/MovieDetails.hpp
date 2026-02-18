#pragma once
#include "Models/MediaDetails.hpp"
#include <string>

namespace TMDBAPI::Models
{
    struct MovieDetails : MediaDetails
    {
        std::string releaseDate{}; 
        std::string imdbId{};
        std::string originalTitle{};
        std::string title{};
        int budget{0};
        int revenue{0};
        int runtime{0};

        ModelClassDeffinitions(MovieDetails)
    };
}