#include "Models/CreditJob.hpp"
#include <json/value.h>

namespace TMDBAPI::Models 
{
    CreditJob::CreditJob(const Json::Value& json)
    {
        if (!json.find("credit_id") || json["credit_id"].isNull())
            throw std::invalid_argument("");
        creditId = json["credit_id"].asString();
        
        if (!json.find("episode_count") || json["episode_count"].isNull())
            throw std::invalid_argument("");
        episodeCount = json["episode_count"].asInt();

        if (json.find("job") && !json["job"].isNull())
            value = json["job"].asString();
        else if (json.find("character") && !json["character"].isNull())
            value = json["character"].asString();
    }
}