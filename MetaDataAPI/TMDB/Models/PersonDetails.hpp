#pragma once
#include "Enums/Enums.hpp"
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <string>
#include <vector>
namespace TMDBAPI::Models
{
    struct PersonDetails : Cacheable
    {
        int id{};
        bool adult{true};
        std::vector<std::string> alsoKnownFor{};
        std::string biography{};
        std::string birthday{};
        std::string deathday{};
        Gender gender{Gender::NotSet};
        std::string homepage{};
        std::string imdbId{};
        Department knownForDepartment{};
        std::string name{};
        std::string placeOfBirth{};
        float popularity{};
        std::string profilePath{};
        ModelClassDeffinitions(PersonDetails)
    };
}
// Нужне Partial