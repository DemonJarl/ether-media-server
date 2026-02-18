#include "Models/AggregateCrewCredit.hpp"

namespace TMDBAPI::Models 
{
    AggregateCrewCredit::AggregateCrewCredit(const Json::Value &json) 
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();

        if(!json.find("adult") || json["adult"].isNull())
            throw std::invalid_argument("");
        adult = json["adult"].asBool();

        if(!json.find("known_for_department"))
            throw std::invalid_argument("");
        if(!json["known_for_department"].isNull())
            knownForDepartament = stringToDepartment(json["known_for_department"].asString());
        
        if(!json.find("gender"))
            throw std::invalid_argument("");
        if(!json["gender"].isNull())
            gender = static_cast<Gender>(json["gender"].asInt());

        // if(!json.find("credit_id"))
        //     throw std::invalid_argument("");
        // if(!json["credit_id"].isNull())
        //     creditId = json["credit_id"].asString();

        // if(!json.find("character"))
        //     throw std::invalid_argument("");
        // if(!json["character"].isNull())
        //     character = json["character"].asString();

        // if(!json.find("total_episode_count") || json["total_episode_count"].isNull())
        //     throw std::invalid_argument("");
        // totalEpisodeCount = json["total_episode_count"].asInt();

        if(!json.find("original_name") || json["original_name"].isNull())
            throw std::invalid_argument("");
        originalName = json["original_name"].asString();

        if(!json.find("name") || json["name"].isNull())
            throw std::invalid_argument("");
        name = json["name"].asString();
        
        if(!json.find("jobs"))
            throw std::invalid_argument(""); 
        if (!json["jobs"].isNull())
        {
            const Json::Value& arr = json["jobs"];
            jobs.reserve(arr.size());
            for(const Json::Value& job : arr)
                jobs.emplace_back(job);
        }
        
    }
}