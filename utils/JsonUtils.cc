#include "JsonUtils.h"
#include "Utils.h"
#include <cstdint>
#include <drogon/HttpResponse.h>
#include <json/value.h>
#include <string>
#include <trantor/utils/Logger.h>
// Добавить заголовки что бы он не кэшировал ответ
HttpResponsePtr errorResponse(const std::string& errorMessage, ErrorCode errorCode, HttpStatusCode statusCode)
{
    Json::Value ret;
    ret["result"] = "error";
    ret["errorMessage"] = errorMessage;
    ret["errorCode"] = errorCode;
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(statusCode);
    return  resp;
}

Json::Value user(int64_t id, const std::string& login)
{
    Json::Value ret;
    ret["userId"] = id;
    ret["login"] = login;
    return ret;
}

Json::Value getUserBySession(std::string& sessionId)
{
    LOG_DEBUG<<"sessionId "<<sessionId;
    if(isStringEmpty(sessionId))
        return Json::nullValue;
    orm::DbClientPtr db = drogon::app().getDbClient();
    try 
    {
        orm::Result result = db->execSqlSync("select sessions.id, users.id as user_id, users.login as login from sessions inner join users on users.id = sessions.user_id where sessions.token = $1", sessionId);       
        if (result.size() == 0)
            return Json::nullValue;
        auto sqlRes = result[0];
        return user(sqlRes["user_id"].as<int64_t>(), sqlRes["login"].as<std::string>());
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error:" << e.base().what() << std::endl;
        return false;
    }
}

bool validateSession(const std::string& sessionId, Json::Value &user)
{
    if (sessionId.empty())
    {
        return false;
    }

    LOG_DEBUG<<"sessionId "<<sessionId;
    orm::DbClientPtr db = drogon::app().getDbClient();
    try
    {
        orm::Result result = db->execSqlSync("select sessions.id, users.id as user_id , users.name as name from sessions inner join users on users.id = sessions.user_id where sessions.token = $1", sessionId); // Block until we get the result or catch the exception;
        if (result.size() == 0)
        {
            return false;
        }
        user.clear(); // на всякий
        LOG_DEBUG<<"userId: "<<result[0]["user_id"].as<int64_t>();
        user["userId"] = result[0]["user_id"].as<int64_t>();
        user["login"] = result[0]["name"].as<std::string>();
        return true;   
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error:" << e.base().what() << std::endl;
        return false;
    }
}
Json::Value file(const std::string& name, const std::string& path, const std::string& type, const std::string& extension)
{
    Json::Value ret;
    ret["name"] = name;
    ret["path"] = path;
    ret["type"] = type;
    if (type == "File")
        ret["extension"] = extension;
    return ret;
}


bool validateUserJson(Json::Value& json, JsonValidationMode mode)
{
    switch (mode) 
    {
        case JsonValidationMode::Creation:
            //json.isMember()
            break;

        case JsonValidationMode::Update:

            break;

        case JsonValidationMode::Patch:
            break;

    }
    return true;
}
bool validateLibraryJson(Json::Value& json, JsonValidationMode mode)
{
    return true;
}
bool validatePathJson(Json::Value& json, JsonValidationMode mode)
{
    return true;
}
// Json::Value defaultLibrarySetting()
// {
//     Json::Value ret;
//     ret["MetaDataProviders"] = Json::Value();
//     ret["MetaDataProviders"].append("TMDB");
//     return ret;
// }
// Json::Value defaultUserSettings()
// {
//     Json::Value ret, tmp;
//     tmp["size"] = 10;
//     tmp["fontColor"] = "white";
//     tmp["backgroundColor"] = "black";
//     tmp["font"] = "Arial";
//     tmp["backgroundOpacity"] = 10;
//     tmp["foregroundOpacity"] = 10;
//     ret["subtitleSettings"] = tmp;
//     ret["preferedSubtitleLanguage"] = "rus";
//     ret["preferedVOLanguage"] = "rus";
//     return ret;
// }