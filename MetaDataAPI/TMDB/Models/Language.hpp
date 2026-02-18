#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>
namespace TMDBAPI::Models
{
struct Language : Cacheable
{
    std::string iso639_1{};
    std::string englishName{};
    std::string name{};

    ModelClassDeffinitions(Language)
};
}