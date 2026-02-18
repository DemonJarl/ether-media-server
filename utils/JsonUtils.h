#pragma once
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <drogon/RequestStream.h>
#include <json/value.h>
#include <string>
#include <trantor/utils/Logger.h>
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
HttpResponsePtr errorResponse(const std::string& errorMessage = "", ErrorCode errorCode = ErrorCode::IncorectData, HttpStatusCode statusCode = HttpStatusCode::k400BadRequest);
Json::Value user(int64_t id = 0, const std::string& login = "");
Json::Value file(const std::string& name, const std::string& path, const std::string& type, const std::string& extension = "");
// Json::Value defaultUserSettings();
//Json::Value defaultLibrarySetting();
Json::Value getUserBySession(std::string& sessionId);

bool validateUserJson(Json::Value& json, JsonValidationMode mode = JsonValidationMode::Patch);
bool validateLibraryJson(Json::Value& json, JsonValidationMode mode = JsonValidationMode::Patch);
bool validatePathJson(Json::Value& json, JsonValidationMode mode = JsonValidationMode::Patch);