#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include "Models/PartialMovieDetails.hpp"
#include "Models/PartialTVDetails.hpp"
#include "Models/SearchPersonDetails.hpp"
#include <variant>

namespace TMDBAPI::Models 
{
    struct MultiSeach : Cacheable
    {
        std::variant<Models::SearchPersonDetails, Models::PartialTVDetails, Models::PartialMovieDetails> value;

        ModelClassDeffinitions(MultiSeach)
    };
}