#include "Models/PartialEpisode.hpp"
namespace TMDBAPI::Models 
{
    PartialEpisode::PartialEpisode(const Json::Value& json) : BaseEpisode(json)
    {
        if (!json.find("show_id"))
            throw std::invalid_argument("");
        if (!json["show_id"].isNull())
            showId = json["show_id"].asInt();
    }
}