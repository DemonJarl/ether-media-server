#pragma once
#include "Models/ModelDefinition.hpp"
namespace TMDBAPI::Models
{
struct Country// : BaseEntity
{
    std::string iso3166_1{};
    std::string englishName{};
    std::string nativeName{}; 
    ModelClassDeffinitions(Country)
};
}