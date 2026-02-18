#pragma once
#include "Models/Cacheable.hpp"
#include "Models/Image.hpp"
#include <vector>

namespace TMDBAPI::Models 
{
    struct PartialEpisodeMediaImages : Cacheable
    {
        std::vector<Image> stills{};
        ModelClassDeffinitions(PartialEpisodeMediaImages)
    };
}