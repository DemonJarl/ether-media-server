#include "SearchPersonDetails.hpp"

namespace TMDBAPI::Models 
{
    SearchPersonDetails::SearchPersonDetails(const Json::Value& json)
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();

        if (!json.find("known_for"))
            throw std::invalid_argument("");
        if (!json["known_for"].isNull())
        {
            const Json::Value& arr = json["known_for"];
            knownFor.reserve(arr.size());
            for(const Json::Value& credit : arr)
            {
                knownFor.emplace_back(credit);
            }
        }

        if(!json.find("adult") || json["adult"].isNull())
            throw std::invalid_argument("");
        adult = json["adult"].asBool();

        if(!json.find("known_for_department"))
            throw std::invalid_argument("");
        if(!json["known_for_department"].isNull())
            knownForDepartment = stringToDepartment(json["known_for_department"].asString());
        
        if(!json.find("gender"))
            throw std::invalid_argument("");
        if(!json["gender"].isNull())
            gender = static_cast<Gender>(json["gender"].asInt());

        if(!json.find("original_name") || json["original_name"].isNull())
            throw std::invalid_argument("");
        originalName = json["original_name"].asString();

        if(!json.find("name") || json["name"].isNull())
            throw std::invalid_argument("");
        name = json["name"].asString();

        if(!json.find("popularity") || json["popularity"].isNull())
            throw std::invalid_argument("");
        popularity = json["popularity"].asFloat();

        if(!json.find("profile_path") || json["profile_path"].isNull())
            throw std::invalid_argument("");
        profilePath = json["profile_path"].asString();
    }
}