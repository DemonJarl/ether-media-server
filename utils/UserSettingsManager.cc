#include "UserSettingsManager.hpp"

const std::string UserSettingsManager::Fields::size = "size";
const std::string UserSettingsManager::Fields::fontColor = "fontColor";
const std::string UserSettingsManager::Fields::font = "font";
const std::string UserSettingsManager::Fields::backgroundColor = "backgroundColor";
const std::string UserSettingsManager::Fields::backgroundOpacity = "backgroundOpacity";
const std::string UserSettingsManager::Fields::foregroundOpacity = "foregroundOpacity";
const std::string UserSettingsManager::Fields::subtitleSettings = "subtitleSettings";
const std::string UserSettingsManager::Fields::preferedSubtitleLanguage = "preferedSubtitleLanguage";
const std::string UserSettingsManager::Fields::preferedVOLanguage = "preferedVOLanguage";
const std::string UserSettingsManager::Fields::UILanguage = "UILanguage";

Json::Value UserSettingsManager::defaultUserSettings()
{
    Json::Value ret, tmp;
    tmp[Fields::size] = 14;
    tmp[Fields::fontColor] = "white";
    tmp[Fields::backgroundColor] = "black";
    tmp[Fields::font] = "Arial";
    tmp[Fields::backgroundOpacity] = 1.f;
    tmp[Fields::foregroundOpacity] = 0.f;
    ret[Fields::subtitleSettings] = tmp;
    ret[Fields::UILanguage] = enumToInt(Language::ru);
    ret[Fields::preferedSubtitleLanguage] = enumToInt(Language::ru);
    ret[Fields::preferedVOLanguage] = enumToInt(Language::ru);
    return ret;
}