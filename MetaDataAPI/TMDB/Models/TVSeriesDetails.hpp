#pragma once
#include "Genre.hpp"
#include "MediaDetails.hpp"
#include "Models/PartialCompanyNetwork.hpp"
#include "Models/PartialEpisode.hpp"
#include "Models/PartialSeason.hpp"
#include "Models/Person.hpp"
#include <json/value.h>
#include <optional>
#include <string>
#include <vector>
namespace TMDBAPI::Models
{
struct TVSeriesDetails : MediaDetails
{
    std::vector<PersonDetails> createdBy{};
    std::vector<int> episodeRuntimes{};
    std::string firstAirDate{};
    bool inProduction{true};
    std::string name{};
    std::string originalName{};
    std::vector<std::string> languages{};//PartialLanguage
    int numberOfEpisodes{0};
    int numberOfSeasons{0};
    std::vector<Genre> genres{};
    std::vector<PartialSeason> seasons{};
    std::optional<PartialEpisode> lastAiredEpisode{};
    std::vector<PartialCompanyNetwork> networks{};
    std::string type{};
    ModelClassDeffinitions(TVSeriesDetails)
};
}