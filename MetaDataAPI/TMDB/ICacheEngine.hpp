#pragma once
#include "Models/Cacheable.hpp"
#include <json/value.h>
#include <memory>
#include <optional>
#include <iostream>
namespace TMDBAPI 
{
    class ICacheEngine 
    {
    public:
        virtual std::optional<Json::Value> getCache(const std::string& url) = 0;
        virtual void putCache(const std::string& url, const Json::Value& cache) = 0;
        virtual void clearCache() = 0;
        virtual ~ICacheEngine() = default;

        // ICacheEngine& operator=(const ICacheEngine&& other) = delete;
        // ICacheEngine& operator=(const ICacheEngine& other) = delete;
        // ICacheEngine(const ICacheEngine&& other) = delete;
        // ICacheEngine(const ICacheEngine& other) = delete;
    };
}