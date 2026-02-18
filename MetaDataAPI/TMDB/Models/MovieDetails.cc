#include "Models/MovieDetails.hpp"
#include "Models/MediaDetails.hpp"
#include <boost/locale/encoding.hpp>
#include <codecvt>
#include <iostream>
#include <string>
#include <trantor/utils/Logger.h>
#include "boost/algorithm/string/trim.hpp"
#include "boost/locale.hpp"
#include <boost/locale/encoding_utf.hpp>
#include <boost/locale/conversion.hpp>
#include <boost/locale/utf.hpp>
namespace TMDBAPI::Models
{
MovieDetails::MovieDetails(const Json::Value& json) : MediaDetails(json)
{
    if (!json.find("budget"))
            throw std::invalid_argument("");
    budget = json["budget"].asInt();

    if (!json.find("imdb_id"))
            throw std::invalid_argument("");
    imdbId = json["imdb_id"].asString();

    if (!json.find("title"))
        throw std::invalid_argument("");
    title = json["title"].asString();

    if (!json.find("original_title"))
        throw std::invalid_argument("");
    originalTitle = json["original_title"].asString();
    
    if (!json.find("revenue"))
        throw std::invalid_argument("");
    revenue = json["revenue"].asInt();

    if (!json.find("runtime"))
        throw std::invalid_argument("");
    runtime = json["runtime"].asInt();

    if (!json.find("release_date"))
        throw std::invalid_argument("");
    releaseDate = json["release_date"].asString();
}
}