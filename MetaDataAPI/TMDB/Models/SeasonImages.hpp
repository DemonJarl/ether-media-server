#pragma once
#include "Models/Cacheable.hpp"
#include "Models/Image.hpp"
namespace TMDBAPI::Models 
{
    struct SeasonImages : Cacheable
    {
        int id{};
        std::vector<Image> posters{};
        ModelClassDeffinitions(SeasonImages)
    };
}