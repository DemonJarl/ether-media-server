#include "Models/CrewCredit.hpp"

namespace TMDBAPI::Models 
{
    CrewCredit::CrewCredit(const Json::Value &json) 
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();

        // if (!json.find("order") || json["order"].isNull())
        //     throw std::invalid_argument("");
        // order = json["order"].asInt();

        if(!json.find("credit_id") || json["credit_id"].isNull())
            throw std::invalid_argument("");
        creditID = json["credit_id"].asString();

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

        if(!json.find("job"))
            throw std::invalid_argument("");
        if(!json["job"].isNull())
            job = json["job"].asString();

        if(!json.find("department"))
            throw std::invalid_argument("");
        if(!json["department"].isNull())
            department = stringToDepartment(json["department"].asString());

        if(!json.find("original_name") || json["original_name"].isNull())
            throw std::invalid_argument("");
        originalName = json["original_name"].asString();

        if(!json.find("name") || json["name"].isNull())
            throw std::invalid_argument("");
        name = json["name"].asString();

        if(!json.find("profile_path"))
            throw std::invalid_argument("");
        profilePath = json["profile_path"].asString();

        if(!json.find("popularity") || json["popularity"].isNull())
            throw std::invalid_argument("");
        popularity = json["popularity"].asFloat();
        
    }
}