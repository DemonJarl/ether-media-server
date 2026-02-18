#pragma once
#include "Models/Cacheable.hpp"
#include "Models/Image.hpp"
#include "PartialMediaImages.hpp"
#include <vector>

namespace TMDBAPI::Models 
{
    struct MediaImages : PartialMediaImages
    {
        int id{};
        ModelClassDeffinitions(MediaImages)
    };
}