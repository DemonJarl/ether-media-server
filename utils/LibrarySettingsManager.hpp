#pragma once
#include <cstdint>
#include <json/value.h>
#include <string>
#include <json/json.h>
#include "EnumToInt.h"
#include "MediaEnums.h"
#include <drogon/HttpAppFramework.h>
#include <vector>

// Json::Value defaultLibrarySetting();
// Добавить метод для валидации json
class LibrarySettingsManager
{
public:
    struct Fields
    {   
        static const std::string metadataProviders;
    };
private:
    Json::Value settings;
public:
    LibrarySettingsManager(const Json::Value& json)
    {
        settings = json;
    }
    LibrarySettingsManager()
    {
        settings = defaultLibrarySetting();
    }
    void resetSettings()
    {
        settings = defaultLibrarySetting();
    }
    void clearMetadataProviders()
    {
        settings[Fields::metadataProviders] = Json::nullValue;
    }
    void addProvider(const MetaDataProvider provider)
    {
        //settings[LibrarySettingsFields::metadataProviders]
    }
    void subProvider(const MetaDataProvider provider)
    {
        settings[Fields::metadataProviders] = enumToInt(removeFlag(static_cast<MetaDataProvider>(settings[Fields::metadataProviders].as<int64_t>()), provider));
    }
    static Json::Value defaultLibrarySetting();
};