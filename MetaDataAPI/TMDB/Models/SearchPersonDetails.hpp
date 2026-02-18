#pragma once
#include "Enums/Enums.hpp"
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include "Models/PartialMediaDetails.hpp"
#include <vector>

namespace TMDBAPI::Models 
{
    struct SearchPersonDetails : Cacheable
    {
        int id{};
        bool adult{};
        Gender gender{Gender::NotSet};
        Department knownForDepartment{};
        std::string name{};
        std::string originalName{};
        float popularity{};
        std::string profilePath{};
        std::vector<Models::PartialMediaDetails> knownFor{};
        ModelClassDeffinitions(SearchPersonDetails)
    };
}