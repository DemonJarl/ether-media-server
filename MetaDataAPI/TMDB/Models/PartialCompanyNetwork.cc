#include "Models/PartialCompanyNetwork.hpp"
namespace TMDBAPI::Models 
{
    PartialCompanyNetwork::PartialCompanyNetwork(const Json::Value& json) 
    {
        if (!json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");
        id = json["id"].asInt();
        if (!json.find("logo_path"))
            throw std::invalid_argument("");
        if (!json["logo_path"].isNull())
            logoPath = json["logo_path"].asString();
        
        if (!json.find("name") || json["name"].isNull())
            throw std::invalid_argument("");
        name = json["name"].asString();

        if (!json.find("origin_country"))
            throw std::invalid_argument("");
        if (!json["origin_country"].isNull())
            originCountry = json["origin_country"].asString();
    }
}