#include "api_Users.h"
#include "drogon/orm/DbClient.h"
#include <cstdint>
#include <ctime>
#include <drogon/Cookie.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/orm/Result.h>
#include <drogon/utils/Utilities.h>
#include <json/value.h>
#include <string>
#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <iostream>
#include "utils/JsonUtils.h"
#include "utils/RequestHeaderUtils.h"
using namespace api;

void Users::me(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback)
{
    //Json::Value ret = user();
    std::string session = getAuthToken(req->getHeader("Authorization"));
    if (session.empty()) // На всякий
    {
        callback(errorResponse("Не корректный токен"));
        return;
    }
    Json::Value ret = getUserBySession(session);
    if (ret == Json::nullValue)
    {
        callback(errorResponse(" Не удалось получить с такой сессией"));
        return;
    }
    callback(HttpResponse::newHttpJsonResponse(ret));
    return;
}


void Users::getUsers(const HttpRequestPtr &req,
                std::function<void (const HttpResponsePtr &)> &&callback)
{
    orm::DbClientPtr db = drogon::app().getDbClient();
    try
    {
        orm::Result result = db->execSqlSync("select login, id from users"); // Block until we get the result or catch the exception;
        if (result.size() == 0)
        {
            callback(errorResponse("Пользователь не найден", ObjectNotFound, HttpStatusCode::k404NotFound));
            return;
        }
        Json::Value ret;
        for (auto row : result)
        {
            Json::Value user;
            user["userId"] = row["id"].as<std::string>();
            user["userName"] = row["login"].as<std::string>();
            ret.append(user);
        }
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
        return;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        LOG_ERROR<< "error:" << e.base().what();
        callback(errorResponse("Database error", DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
}

void Users::getUser(const HttpRequestPtr &req,
                std::function<void (const HttpResponsePtr &)> &&callback,
                std::string userId)
{
    if (userId.size() == 0)
    {
        callback(errorResponse());
        return;
    }
    orm::DbClientPtr db = drogon::app().getDbClient();
    try
    {
        orm::Result result = db->execSqlSync("select login, users.id from users where id=$1" , userId); // Block until we get the result or catch the exception;
        if (result.size() == 0)
        {
            callback(errorResponse("Пользователь не найден", ObjectNotFound, HttpStatusCode::k404NotFound));
            return;
        }
        Json::Value ret;
        ret["userId"] = userId;
        ret["userName"] = result[0]["login"].as<std::string>();//Надо сделать трим функцию
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
        return;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        LOG_ERROR<< "error:" << e.base().what();
        callback(errorResponse("Database error", DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
}

void Users::deleteUser(const HttpRequestPtr &req,
                std::function<void (const HttpResponsePtr &)> &&callback)
{
    std::string userId = req->getParameter("userId");
    if (userId.size() == 0)
    {
        callback(errorResponse("Не предоставлены данные", ObjectNotFound));
        return;
    }
    orm::DbClientPtr db = drogon::app().getDbClient();
    try
    {
        orm::Result result = db->execSqlSync("delete from users where id=$1 RETURNING id" , userId); // Block until we get the result or catch the exception;
        if (result.size() == 0)
        {
            callback(errorResponse("Пользователь не найден", ObjectNotFound,  HttpStatusCode::k404NotFound));
            return;
        }
        auto resp=HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
        return;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        LOG_ERROR<< "error:" << e.base().what();
        callback(errorResponse("Database error", DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
}

void Users::updateUser(const HttpRequestPtr &req,
                std::function<void (const HttpResponsePtr &)> &&callback,
                std::string userId)
{
    
}