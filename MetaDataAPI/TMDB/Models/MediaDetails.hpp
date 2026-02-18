#pragma once
#include "Models/ModelDefinition.hpp"
#include "Models/CollectionDetails.hpp"
#include "Models/Genre.hpp"
#include "Models/Language.hpp"
#include "Models/PartialCompanyNetwork.hpp"
#include "Models/PartialCountry.hpp"
#include <json/value.h>
#include <string>
#include <vector>
namespace TMDBAPI::Models
{
struct MediaDetails : Cacheable
{
    int id{};
    bool adult{true};
    std::string backdropPath{};
    std::vector<Genre> genres{};
    std::vector<Language> spokenLanguages{};

    std::vector<PartialCompanyNetwork> productionCompanies{};
    std::vector<PartialCountry> productionCountries{};
    std::optional<CollectionDetails> bellongsToCollection{};
    std::string homepage{};
    std::string originalLanguage{};

    std::string overview{};
    std::string tagline{};
    float popularity{};
    std::string posterPath{};
    std::string status{};

    bool video{};
    float voteAverage{0};
    int voteCount{0};
    
    ModelClassDeffinitions(MediaDetails)
};
}