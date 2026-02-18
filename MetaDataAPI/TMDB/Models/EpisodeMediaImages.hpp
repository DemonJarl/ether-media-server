#pragma once
#include "Models/PartialEpisodeMediaImages.hpp"
namespace TMDBAPI::Models 
{
    struct EpisodeMediaImages : PartialEpisodeMediaImages
    {
        int id{};
        ModelClassDeffinitions(EpisodeMediaImages)
    };
}