#pragma once
#include "CastCredit.hpp"
#include "CrewCredit.hpp"
#include "Models/PartialCredits.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>
#include <vector>

namespace TMDBAPI::Models 
{
    struct Credits : PartialCredits
    {
        int id;
        ModelClassDeffinitions(Credits)
    };
}