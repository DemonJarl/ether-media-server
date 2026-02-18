#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>
namespace TMDBAPI::Models
{
    struct PartialCompanyNetwork : Cacheable
    {
        int id{};
        std::string logoPath{};
        std::string name{};
        std::string originCountry{};
        ModelClassDeffinitions(PartialCompanyNetwork)
    };
}