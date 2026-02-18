#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models 
{
    struct CreditJob : Cacheable
    {
        std::string creditId{};
        std::string value{};
        int episodeCount{};
        ModelClassDeffinitions(CreditJob)
    };
}