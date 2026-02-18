#pragma once

#include "Enums/Enums.hpp"
#include "Models/Cacheable.hpp"
#include "Models/PartialMovieDetails.hpp"
#include "Models/PartialTVDetails.hpp"
#include "Models/SearchPersonDetails.hpp"
#include <iostream>
#include <json/value.h>
#include <type_traits>
#include <vector>
namespace TMDBAPI::Models 
{



    template<class Model>
    struct Page : Cacheable
    {
    
        int pageNum{1};
    public:
        //static_assert(std::is_base_of_v<Cacheable, Model>, "Model should be derived from Cacheable");
        std::vector<Model> results{};
        int getPageNum() const
        {
            return pageNum;
        }
        Page<Model>(const Json::Value& json)
        {
            if (!json.isMember("page") || json["page"].isNull())
                throw std::invalid_argument("");
            pageNum = json["page"].asInt();

            if (!json.isMember("results") || json["results"].isNull())
                throw std::invalid_argument("");
            Json::Value res = json["results"];
            results.reserve(res.size());
            for (const Json::Value& result : res)
            {
                results.emplace_back(result); // Как?
            }
        }
    };
}