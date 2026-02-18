#include "Models/PersonDetails.hpp"
#include "Enums/Enums.hpp"
namespace TMDBAPI::Models
{
    PersonDetails::PersonDetails(const Json::Value &json) 
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();

        if(!json.find("adult") || json["adult"].isNull())
            throw std::invalid_argument("");
        adult = json["adult"].asBool();

        if(!json.find("also_known_as"))
            throw std::invalid_argument("");
        if(!json["also_known_as"].isNull())
        {
            const Json::Value& arr = json["also_known_as"];
            alsoKnownFor.reserve(arr.size());
            for (const Json::Value& known : arr)
                alsoKnownFor.push_back(known.asString());
        }

        if(!json.find("biography"))
            throw std::invalid_argument("");
        if(!json["biography"].isNull())
            biography = json["biography"].asString();

        if(!json.find("birthday"))
            throw std::invalid_argument("");
        if(!json["birthday"].isNull())
            birthday = json["birthday"].asString();

        if(!json.find("deathday"))
            throw std::invalid_argument("");
        if(!json["deathday"].isNull())
            deathday = json["deathday"].asString();

        if(!json.find("gender"))
            throw std::invalid_argument("");
        if(!json["gender"].isNull())
            gender = static_cast<Gender>(json["gender"].asInt());

        if(!json.find("homepage"))
            throw std::invalid_argument("");
        if(!json["homepage"].isNull())
            homepage = json["homepage"].asString();

        if(!json.find("imdb_id"))
            throw std::invalid_argument("");
        if(!json["imdb_id"].isNull())
            imdbId = json["imdb_id"].asString();

        if(!json.find("name"))
            throw std::invalid_argument("");
        if(!json["name"].isNull())
            name = json["name"].asString();

        if(!json.find("place_of_birth"))
            throw std::invalid_argument("");
        if(!json["place_of_birth"].isNull())
            placeOfBirth = json["place_of_birth"].asString();

        if(!json.find("popularity"))
            throw std::invalid_argument("");
        if(!json["popularity"].isNull())
            popularity = json["popularity"].asFloat();

        if(!json.find("profile_path"))
            throw std::invalid_argument("");
        if(!json["profile_path"].isNull())
            profilePath = json["profile_path"].asString();
    }
}