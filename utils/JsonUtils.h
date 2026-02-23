#pragma once
#include "Credits.h"
#include "ExternalMediaItemIds.h"
#include "Images.h"
#include "MediaEnums.h"
#include "MediaItemStreams.h"
#include "MediaItems.h"
#include "coro/task.hpp"
#include <cstdint>
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <drogon/RequestStream.h>
#include <json/value.h>
#include <string>
#include <trantor/utils/Logger.h>
#include <models/Libraries.h>
#include <vector>
using namespace drogon; 
enum ErrorCode
{
    IncorectData = 0,
    InvalidLoginInfo = 1,
    InvalidAuthToken = 2,
    DataBaseError = 3,
    ObjectNotFound = 4,
    EmptyData = 5,
};
enum JsonValidationMode
{
    Creation,
    Update,
    Patch
};

namespace models = drogon_model::sqlite3;

HttpResponsePtr errorResponse(const std::string& errorMessage = "", ErrorCode errorCode = ErrorCode::IncorectData, HttpStatusCode statusCode = HttpStatusCode::k400BadRequest);
Json::Value user(int64_t id = 0, const std::string& login = "");
Json::Value file(const std::string& name, const std::string& path, const std::string& type, const std::string& extension = "");
// Json::Value defaultUserSettings();
//Json::Value defaultLibrarySetting();
Json::Value getUserBySession(std::string& sessionId);

bool validateUserJson(Json::Value& json, JsonValidationMode mode = JsonValidationMode::Patch);
bool validateLibraryJson(Json::Value& json, JsonValidationMode mode = JsonValidationMode::Patch);
bool validatePathJson(Json::Value& json, JsonValidationMode mode = JsonValidationMode::Patch);

coro::task<Json::Value> libraryToJson(const models::Libraries& library);
coro::task<Json::Value> libraryToJson(const int64_t libraryID);

coro::task<Json::Value> mediaItemToJson(const int64_t mediaItemID, const Language language);
coro::task<Json::Value> mediaItemToJson(const models::MediaItems& mediaItem, const Language language);

Json::Value mediaStreamToJson(const models::MediaItemStreams& stream);

Json::Value imageToJson(const models::Images& image);

Json::Value creditToJson(const models::Credits& credit);

Json::Value externalMediaItemIDToJson(const models::ExternalMediaItemIds& extId);

inline bool findAndCheckNullArray(const Json::Value& json, const std::string& key)
{
    if(!json.find(key))
        return false;
    const Json::Value& check = json[key];
    return !check.empty() && check.isArray();
}


inline bool findAndCheckNull(const Json::Value& json, const std::string& key)
{
    return json.find(key) && !json[key].isNull();
}

template<typename T>
inline bool findAndCheckNullAndType(const Json::Value& json, const std::string& key)
{
    if (!json.find(key))
        return false;
    const Json::Value& check = json[key];
    return !check.empty() && check.is<T>();
}