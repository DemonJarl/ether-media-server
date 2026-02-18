#pragma once
#include <cstdint>
#include <json/value.h>
#include <string>
#include <json/json.h>
#include "EnumToInt.h"
#include "MediaEnums.h"
#include <drogon/HttpAppFramework.h>
#include <vector>


class UserSettingsManager
{
public:
    struct Fields
    {   
        static const std::string size;
        static const std::string fontColor;
        static const std::string font;
        static const std::string backgroundColor;
        static const std::string backgroundOpacity;
        static const std::string foregroundOpacity;
        static const std::string subtitleSettings;
        static const std::string preferedSubtitleLanguage;
        static const std::string preferedVOLanguage;
        static const std::string UILanguage;
    };
private:
    Json::Value settings;
public:
    UserSettingsManager(const Json::Value& json)
    {
        settings = json;
    }
    UserSettingsManager()
    {
        settings = defaultUserSettings();
    }
    void resetSettings()
    {
        settings = defaultUserSettings();
    }
    void setVOLanguage(const Language language)
    {
        if (language == Language::xx)
            return;
        settings[Fields::preferedVOLanguage] = enumToInt(language);
    }
    void setSubtitleLanguage(const Language language)
    {
        if (language == Language::xx)
            return;
        settings[Fields::preferedSubtitleLanguage] = enumToInt(language);
    }
    void setUILanguage(const Language language)
    {
        if (language == Language::xx)
            return;
        settings[Fields::UILanguage] = enumToInt(language);
    }
    static Json::Value defaultUserSettings();
};