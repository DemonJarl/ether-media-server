#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <json/value.h>
#include <string>
namespace TMDBAPI::Models
{
struct Genre : Cacheable
{
    int id{};
    std::string name{};
    ModelClassDeffinitions(Genre)
};
}