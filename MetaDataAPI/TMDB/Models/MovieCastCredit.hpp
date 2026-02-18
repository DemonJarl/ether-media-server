#pragma once
#include "Enums/Enums.hpp"
#include "Models/CastCredit.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models 
{
    struct MovieCastCredit : CastCredit
    {
        int castId{};
        ModelClassDeffinitions(MovieCastCredit)
    };
}