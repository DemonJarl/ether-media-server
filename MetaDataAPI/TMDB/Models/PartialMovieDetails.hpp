#pragma once
#include "Enums/Enums.hpp"
#include "Models/Cacheable.hpp"
#include "Models/ModelDefinition.hpp"
#include <vector>
namespace TMDBAPI::Models 
{
    // Используется в Search и Collection details
    struct PartialMovieDetails : Cacheable
    {
        int id{};
        bool adult{true};
        std::string backdropPath{};
        std::string name{};
        // Почему то это поле то original_title, то original_name
        // У меня это будет original_title, потому что я не хочу делать ещё один struct
        std::string originalTitle{};

        std::string overview{};

        std::string title{};
        std::string posterPath{};
        MediaType mediaType{MediaType::Movie};
        std::string originalLanguage{};
        std::vector<int> genreIds{};

        float popularity{};
        std::string releaseDate{};
        bool video{};
        float voteAverage{0};
        int voteCount{0};

        ModelClassDeffinitions(PartialMovieDetails)
    };
}