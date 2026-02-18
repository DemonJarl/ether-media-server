#pragma once
#include "PartialSeasonMediaImages.hpp"

namespace TMDBAPI::Models 
{
    struct SeasonMediaImages : PartialSeasonMediaImages
    {
        int id{};
        ModelClassDeffinitions(SeasonMediaImages)
    };
}