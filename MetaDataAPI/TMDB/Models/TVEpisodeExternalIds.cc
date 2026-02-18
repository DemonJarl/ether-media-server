#include "Models/TVEpisodeExternalIds.hpp"
namespace TMDBAPI::Models 
{
    TVEpisodeExternalIds::TVEpisodeExternalIds(const Json::Value& json) : PartialTVEpisodeExternalIds(json)
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();
    }
}