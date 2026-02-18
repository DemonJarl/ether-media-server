#include "api_Auth.h"
#include "UserSettingsManager.hpp"
#include "drogon/orm/DbClient.h"
#include <cstdint>
#include <ctime>
#include <drogon/Cookie.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/Result.h>
#include <drogon/utils/Utilities.h>
#include <json/value.h>
#include <string>
#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <iostream>
#include "bcrypt.h"
#include "utils/JsonUtils.h"
#include "models/Users.h"
#include <drogon/orm/Mapper.h>
#include "utils/SessionUtils.h"
#include "utils/RequestHeaderUtils.h"

using namespace api;
namespace models = drogon_model::sqlite3;

void Auth::logout(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback)
{
    // Удалить сессию из БД
    callback(errorResponse("Logout"));
    return;
}
void Auth::login(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback)
{
    
    auto jsonReq = req->getJsonObject();
    if (jsonReq->empty())
    {
        callback(errorResponse("No data", EmptyData));
        return;
    }
    if (!jsonReq->isMember("login") || !jsonReq->isMember("password"))
    {
        callback(errorResponse("", ErrorCode::InvalidLoginInfo, HttpStatusCode::k400BadRequest));
        return;
    }
    std::string login = jsonReq->get("login",Json::nullValue).asString();
    std::string password = jsonReq->get("password",Json::nullValue).asString();
    Json::Value ret;
    if (login.length() == 0 || password.length() == 0)
    {
        callback(errorResponse());
        return;
    }
    orm::DbClientPtr db = drogon::app().getDbClient();
    try
    {
        // orm::Mapper<models::Users> mp(db);
        // mp.findOne()
        orm::Result result = db->execSqlSync("select * from users where login=$1", login); // Block until we get the result or catch the exception;
        if (result.size() == 0)
        {
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k404NotFound);
            callback(resp);
            return;
        }
        std::string dbPassword = result.at(0)["password"].as<std::string>();
        if (!bcrypt::validatePassword(password, dbPassword))
        {
            auto resp=HttpResponse::newHttpJsonResponse(NULL);
            resp->setStatusCode(HttpStatusCode::k401Unauthorized);
            callback(resp);
            return;
        }
        uint64_t userID = result[0]["id"].as<int64_t>();
        ret["userId"]=userID;
        // std::string sessionId = utils::getUuid();
        // result = db->execSqlSync("insert into sessions (user_id, token, device) values ($1, $2, 'test') RETURNING id", userID, sessionId);
        // if (result.size() == 0) // не вернулся id значит не удалось вставить
        // {
        //     // наверное надо вынести в макро
        //     ret["result"]="error";
        //     auto resp=HttpResponse::newHttpJsonResponse(ret);
        //     resp->setStatusCode(HttpStatusCode::k400BadRequest);
        //     callback(resp);
        //     return;
        // }
        std::string sessionID = createSession(userID, getDevice(req->getHeader("User-Agent")));
        if (sessionID.empty())
        {
            ret.clear();
            ret["result"]="error";
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            callback(resp);
            return;
        }
        ret["result"] = "ok";
        ret["token"] = "Bearer "+sessionID;
        // Надо будет добавить проверку что у нас уже есть куки
        Cookie cookie = Cookie("auth.token", sessionID);
        cookie.setMaxAge(31536000);
        //cookie.setHttpOnly(true);
        //cookie.setSecure(true);
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(drogon::k200OK);
        resp->addCookie(cookie);
        //resp->addHeader("Access-Control-Allow-Origin", "*");
        callback(resp);
        return;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error:" << e.base().what() << std::endl;
        callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
}
void Auth::me(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback)
{
    std::string session = getAuthToken(req->getHeader("Authorization"));
    if (session.empty()) // На всякий
    {
        callback(errorResponse("Не корректный токен"));
        return;
    }
    //LOG_DEBUG<<"session: "<<session<<" "<<session.size();
    Json::Value ret = getUserBySession(session);
    if (ret == Json::nullValue)
    {
        callback(errorResponse(" Не удалось получить с такой сессией"));
        return;
    }
    callback(HttpResponse::newHttpJsonResponse(ret));
    return;
}
void Auth::signUp(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback)
{
    auto jsonReq = req->getJsonObject();
    if (jsonReq->empty())
    {
        callback(errorResponse("Пустой JSON"));
        return;
    }
    if (!jsonReq->isMember("login") || !jsonReq->isMember("password"))
    {
        callback(errorResponse("", ErrorCode::InvalidLoginInfo, HttpStatusCode::k400BadRequest));
        return;
    }
    std::string login = jsonReq->get("login",Json::nullValue).asString();
    std::string password = jsonReq->get("password",Json::nullValue).asString();
    Json::Value ret;
    if (login.length() == 0 || password.length() == 0)
    {
        callback(errorResponse("Пустой JSON"));
        return;
    }
    orm::DbClientPtr db = drogon::app().getDbClient();
    try
    {
        orm::Result result = db->execSqlSync("select login from users where login=$1", login); // Block until we get the result or catch the exception;
        ret["result"]="error";
        if (result.size() != 0)
        {
            callback(errorResponse("Пользователь уже существует"));
            return;
        }
        std::string passwordHash = bcrypt::generateHash(password);
        std::string settings = UserSettingsManager::defaultUserSettings().toStyledString();
        result = db->execSqlSync("insert into users (login, password, settings, administrator) values($1, $2, $3, false) returning id", login, passwordHash, settings); // Block until we get the result or catch the exception;
        if (result.affectedRows() == 0) // не вернулся id значит не удалось вставить
        {
            ret["result"]="error";
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            callback(resp);
            return;
        }
        //uint64_t userID = result[0]["id"].as<int64_t>();
        //ret["userId"]=userID;
        ret["result"]="ok";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
        return;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        std::cerr << "error: " << e.base().what() << std::endl;
        callback(errorResponse("Database error", DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
    return;
}
// Add definition of your processing function here
