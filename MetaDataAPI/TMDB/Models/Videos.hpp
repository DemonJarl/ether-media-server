#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include "Models/Video.hpp"
#include <vector>

namespace TMDBAPI::Models 
{
    struct Videos : Cacheable
    {
        int id{};
        std::vector<Video> results{};
        ModelClassDeffinitions(Videos)
    };
}