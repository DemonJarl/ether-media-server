#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models 
{
    struct Video : Cacheable
    {
        // id идет из url
        std::string iso639_1{};
        std::string iso3166_1{};
        std::string name{};
        std::string key{};
        std::string site{};
        int size{};
        std::string type{}; // Переделать на enum
        bool official{true};
        std::string publishedAt{};
        std::string id{};
        ModelClassDeffinitions(Video)
    };
}