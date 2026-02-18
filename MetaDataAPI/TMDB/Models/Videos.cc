#include "Models/Videos.hpp"
#include <json/value.h>
#include <stdexcept>

namespace TMDBAPI::Models 
{
    Videos::Videos(const Json::Value& json)
    {
        if (json.find("id") || json["id"].isNull())
            throw std::invalid_argument("");

        if (json.find("results"))
            throw std::invalid_argument("");
        if (!json["results"].isNull())
        {
            const Json::Value& arr = json["results"];
            results.reserve(arr.size());
            for (const Json::Value& video : arr)
                results.emplace_back(video);
        }
    }
}