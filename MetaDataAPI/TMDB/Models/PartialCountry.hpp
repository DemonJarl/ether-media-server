#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
namespace TMDBAPI::Models
{
struct PartialCountry : Cacheable
{
    std::string iso3166_1{};
    std::string name{}; 
    ModelClassDeffinitions(PartialCountry)
};
}