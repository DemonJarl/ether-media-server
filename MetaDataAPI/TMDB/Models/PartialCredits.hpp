#pragma once
#include "CastCredit.hpp"
#include "CrewCredit.hpp"
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>
#include <vector>

namespace TMDBAPI::Models 
{
    struct PartialCredits : Cacheable
    {
        std::vector<CastCredit> cast;
        std::vector<CrewCredit> crew;
        ModelClassDeffinitions(PartialCredits)
    };
}