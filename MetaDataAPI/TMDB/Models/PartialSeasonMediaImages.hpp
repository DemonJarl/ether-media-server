#pragma once
#include "Models/Cacheable.hpp"
#include "Models/Image.hpp"
#include <vector>

namespace TMDBAPI::Models 
{
    struct PartialSeasonMediaImages : Cacheable
    {
        std::vector<Image> posters{};
        ModelClassDeffinitions(PartialSeasonMediaImages)
    };
}