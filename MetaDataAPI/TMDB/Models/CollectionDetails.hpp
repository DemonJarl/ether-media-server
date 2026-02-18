#pragma once
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include "Models/PartialMovieDetails.hpp"
#include <string>
namespace TMDBAPI::Models
{
struct CollectionDetails : Cacheable
{
    int id{};
    std::string name{};
    std::string originalLanguage{};
    std::string originalName{};
    std::string overview{};
    std::string posterPath{};
    std::string backdropPath{};
    std::vector<PartialMovieDetails> parts;
    ModelClassDeffinitions(CollectionDetails)
};
}