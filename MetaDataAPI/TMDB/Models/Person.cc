#include "Person.hpp"
#include <json/value.h>
#include <optional>
namespace TMDBAPI::Models
{
    Person::Person(const Json::Value& json) : details(json)
    {  
        if (json.find("images") && !json["images"].isNull())
            images = std::make_optional(json["images"]);
        
        if (json.find("external_ids") && !json["external_ids"].isNull())
        {
            externalIDs = std::make_optional(json["external_ids"]);
            if (externalIDs.has_value())
                externalIDs->id = details.id;
        }
    }
}