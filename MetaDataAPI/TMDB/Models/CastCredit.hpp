#pragma once
#include "Enums/Enums.hpp"
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models 
{
    struct CastCredit : Cacheable
    {
        int id{};
        int order{};
        std::string character{}; // Переделать на enum 
        std::string creditID{};
        bool adult{true};
        Gender gender{Gender::NotSet};
        Department knownForDepartament{};
        std::string name{};
        std::string originalName{};
        float popularity{};
        std::string profilePath{};
        ModelClassDeffinitions(CastCredit)
    };
}