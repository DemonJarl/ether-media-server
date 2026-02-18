#pragma once
#include "Models/Cacheable.hpp"
#include "Models/Image.hpp"
#include <vector>

namespace TMDBAPI::Models 
{
    struct PartialMediaImages : Cacheable
    {
        std::vector<Image> backdrops{};
        std::vector<Image> logos{};
        std::vector<Image> posters{};
        ModelClassDeffinitions(PartialMediaImages)
    };
}