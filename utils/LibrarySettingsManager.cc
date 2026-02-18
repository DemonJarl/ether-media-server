#include "LibrarySettingsManager.hpp"

const std::string LibrarySettingsManager::Fields::metadataProviders = "metadataProviders";
Json::Value LibrarySettingsManager::defaultLibrarySetting()
{
    Json::Value ret;
    ret[Fields::metadataProviders] = enumToInt(MetaDataProvider::TMDB);
    return ret;
}