#pragma once
#include "Enums/Enums.hpp"
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include "Models/CreditJob.hpp"
#include <string>
#include <vector>

namespace TMDBAPI::Models 
{
    struct AggregateCrewCredit : Cacheable
    {
        int id{};
        // int order{};
        // std::string character{}; // Переделать на enum 
        // std::string creditId{};
        // int totalEpisodeCount{};
        bool adult{true};
        Gender gender{Gender::NotSet};
        Department knownForDepartament{};
        std::string name{};
        std::string originalName{};
        std::string profilePath{};
        std::vector<CreditJob> jobs{};
        ModelClassDeffinitions(AggregateCrewCredit)
    };
}