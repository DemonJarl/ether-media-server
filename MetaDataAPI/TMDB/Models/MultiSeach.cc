#include "Models/MultiSeach.hpp"
#include "Models/PartialMovieDetails.hpp"
#include <json/value.h>

namespace TMDBAPI::Models 
{
    MultiSeach::MultiSeach(const Json::Value& json) : value(Models::SearchPersonDetails(json))
    {
        this->value = Models::PartialMovieDetails(json);
    }
}