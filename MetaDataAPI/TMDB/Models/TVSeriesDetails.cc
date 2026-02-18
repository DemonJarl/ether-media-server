#include "Models/TVSeriesDetails.hpp"
#include <stdexcept>
namespace TMDBAPI::Models
{
TVSeriesDetails::TVSeriesDetails(const Json::Value& json) : MediaDetails(json)
{
    // if (!json.find("created_by"))
    //         throw std::invalid_argument("");
    // if (!json["created_by"].isNull())
    //     for (const Json::Value &creator : json["created_by"])
    //         createdBy.emplace_back(creator);

    if (!json.find("episode_run_time"))
            throw std::invalid_argument("");
    if (!json["episode_run_time"].isNull())
        for (const Json::Value &runtime : json["episode_run_time"])
            episodeRuntimes.push_back(runtime.asInt());

    if (!json.find("first_air_date"))
        throw std::invalid_argument("");
    if (!json["first_air_date"].isNull())
        firstAirDate = json["first_air_date"].asString();

    if (!json.find("in_production"))
        throw std::invalid_argument("");
    inProduction = json["in_production"].asBool();

    if (!json.find("languages"))
        throw std::invalid_argument("");
    if (!json["languages"].isNull())
        for (const Json::Value &language : json["languages"])
            languages.push_back(language.asString());

    if (!json.find("genres"))
        throw std::invalid_argument("");
    if (!json["genres"].isNull() && !json["genres"].empty())
    {
        const auto& genresJson = json["genres"];
        genres.reserve(genresJson.size());
        for (const auto& genre : genresJson)
            genres.emplace_back(genre);
    }

    if (!json.find("original_name"))
        throw std::invalid_argument("");
    originalName = json["original_name"].asString();

    if (!json.find("name"))
        throw std::invalid_argument("");
    name = json["name"].asString();

    if (!json.find("number_of_episodes"))
        throw std::invalid_argument("");
    numberOfEpisodes = json["number_of_episodes"].asInt();
    if (!json.find("number_of_seasons"))
        throw std::invalid_argument("");
    numberOfSeasons = json["number_of_seasons"].asInt();

    if (!json.find("seasons"))
        throw std::invalid_argument("");
    if (!json["seasons"].isNull())
        for (const Json::Value &season : json["seasons"])
            seasons.emplace_back(season);

    if (!json.find("last_episode_to_air"))
        throw std::invalid_argument("");
    if (!json["last_episode_to_air"].isNull())
        lastAiredEpisode = std::move(PartialEpisode(json["last_episode_to_air"]));
        
    if (!json.find("networks"))
        throw std::invalid_argument("");
    if (!json["networks"].isNull())
        for (const Json::Value &network : json["networks"])
            networks.emplace_back(network);

    type = json["type"].asString();
}
}