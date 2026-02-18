#pragma once
#include "Enums/Enums.hpp"
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>

namespace TMDBAPI::Models 
{
    struct CrewCredit : Cacheable
    {
        bool adult{true};
        std::string creditID{};
        Department department{};
        Gender gender{Gender::NotSet};
        int id{};
        std::string job{}; // Переделать на enum 
        Department knownForDepartament{};
        std::string name{};
        std::string originalName{};
        float popularity{};
        std::string profilePath{};
        ModelClassDeffinitions(CrewCredit)
    };
}