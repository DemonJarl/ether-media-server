#include "api_Libraries.h"
#include "Libraries.h"
#include "LibraryPaths.h"
#include "LibrarySettingsManager.hpp"
#include "MediaUtils.h"
#include "ThreadPool.h"
#include "Users.h"
#include "utils/JsonUtils.h"
#include "utils/Utils.h"
#include <cstdint>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Exception.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/Utilities.h>
#include <future>
#include <json/forwards.h>
#include <json/value.h>
#include <json/writer.h>
#include <memory>
#include <models/Libraries.h>
#include <queue>
#include <string>
#include <sys/types.h>
#include <trantor/utils/Logger.h>
#include <vector>
//extern dp::thread_pool threadPool;
// перейти на uuid?
using namespace api;
namespace models = drogon_model::sqlite3;
void Libraries::getLibrary(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int libraryId)
{
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    std::shared_ptr<orm::Transaction> transaction = dbClientPtr->newTransaction();
    orm::Mapper<models::Libraries> mp(transaction);
    LOG_DEBUG<<"Library ID "<<libraryId;
    models::Libraries library;
    Json::Value ret;
    try 
    {
        library = mp.findByPrimaryKey(libraryId);
    } 
    catch (drogon::orm::DrogonDbException e) 
    {
        callback(errorResponse());
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return;
    }
    auto resp=HttpResponse::newHttpJsonResponse(library.toJson());
    resp->setStatusCode(HttpStatusCode::k200OK);
    callback(resp);
    return;
}
void Libraries::getLibraries(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback)
{
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    orm::Mapper<models::Libraries> mp(dbClientPtr);
    std::vector<models::Libraries> libraries;
    libraries = mp.findAll();
    //libraries = mp.findBy(drogon::orm::Criteria(models::Users::Cols::_id, orm::CompareOperator::In, tmp));
    Json::Value ret = Json::arrayValue;
    for (auto library : libraries)
    {
        ret.append(library.toJson());
    }
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(HttpStatusCode::k200OK);
    callback(resp);
    return;
}
// Добавить передачу путей чтобы потом не вызывать пути
void Libraries::createLibrary(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback)
{
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    std::shared_ptr<orm::Transaction> transaction = dbClientPtr->newTransaction();
    orm::Mapper<models::Libraries> mp(transaction);
    Json::Value ret;
    Json::Value librarySettings = Json::nullValue;
    std::string err;
    Json::Value reqJson = *req->getJsonObject();
    
    // Сделать функцию для валидации настроек библиотеки
    //reqJson["settings"] = defaultLibrarySetting();
    if (reqJson["settings"].empty() || !reqJson["settings"].isObject())
        reqJson["settings"] = LibrarySettingsManager::defaultLibrarySetting().toStyledString();

    if (!models::Libraries::validateJsonForCreation(reqJson, err))
    {
        callback(errorResponse(err, ErrorCode::DataBaseError));
        return;
    }
    models::Libraries library(reqJson);
    try 
    {
        mp.insert   (library);
        // if (reqJson.isMember("paths"))
        //     if (reqJson["paths"].isArray())
        //     {
        //         models::LibraryPaths libraryPath{};
        //         libraryPath.setLibraryId(*library.getId());
        //         //  Надо наверное сделать это транзакцией
        //         for (auto path : reqJson["paths"])
        //         {
        //             orm::Mapper<models::LibraryPaths> mpPaths(dbClientPtr);
        //             std::string sPath = path.asString();
        //             if (sPath.size() == 0)
        //                 continue;
        //             LOG_DEBUG<<"Path "<<sPath;
        //             if (!pathExists(sPath))
        //             {
        //                 LOG_ERROR << "Не существующий путь при создание библиотеки";
        //                 continue;//Или прекращать создавание?
        //             }
        //             libraryPath.setId(utils::getUuid());
        //             libraryPath.setPath(sPath);
        //             // возращать id путей не буду
        //             mpPaths.insert(libraryPath); // Подумать что делать с дубликатами
        //         }
        //     }
    } 
    catch (drogon::orm::UniqueViolation e) { // сделаем пустой обработчик для UniqueViolation (пусть только логирует)
        LOG_WARN<<e.what();
        //std::cerr << "error:" << e.what() << std::endl;
        //callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));
        //return;
    }
    catch (drogon::orm::UnexpectedRows e)
    {
        LOG_ERROR<<e.what();
        transaction->rollback();
    }
    ret["library"]= library.toJson();
    ret["result"] = "ok";
    LOG_DEBUG<<err;
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(HttpStatusCode::k200OK);
    callback(resp);
    return;
}
void Libraries::deleteLibrary(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int64_t libraryId)
{
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    std::shared_ptr<orm::Transaction> transaction = dbClientPtr->newTransaction();
    orm::Mapper<models::Libraries> mp(transaction);
    Json::Value ret;
    ret["result"] = "ok";
    size_t deletitionCount{};
    // Хотя мы можем словить ошибку БД если у нас что то будет с подключением
    try 
    {
        deletitionCount = mp.deleteByPrimaryKey(libraryId);
    } 
    catch (drogon::orm::DrogonDbException e) 
    {
        std::cerr << "error:" << e.base().what() << std::endl;
        transaction->rollback();
        callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    if (deletitionCount == 0)
    {
        resp->setStatusCode(HttpStatusCode::k204NoContent);
        callback(resp);
    }
    else
    {
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
    }
    return;
}
void Libraries::modifyLibrary(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int64_t libraryId)
{
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    std::shared_ptr<orm::Transaction> transaction = dbClientPtr->newTransaction();
    orm::Mapper<models::Libraries> mp(transaction);
    Json::Value reqJson = *req->getJsonObject();
    models::Libraries library;
    try 
    {
        library = mp.findByPrimaryKey(libraryId);
    } 
    catch (drogon::orm::UnexpectedRows e) 
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.what();
        callback(errorResponse("Попытка модификации не существующего объекта", ErrorCode::ObjectNotFound, HttpStatusCode::k404NotFound));
    }
    Json::Value ret;
    std::string err;
    if (library.validateJsonForUpdate(reqJson, err)) // смысл делать операции с бд если они индентичные
    {
        try 
        {
            library.updateByJson(reqJson);
        } 
        catch (const drogon::orm::UnexpectedRows& e) 
        {
            std::cerr << "error:" << e.what() << std::endl;
            callback(errorResponse("", ErrorCode::DataBaseError, HttpStatusCode::k400BadRequest));
        }
        ret["result"] = "ok";
        auto resp=HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
    }
    else {
        LOG_DEBUG<<err;
        transaction->rollback();
        callback(errorResponse("Некоректные данные", ErrorCode::IncorectData, HttpStatusCode::k400BadRequest));
    }
    return;
}

void Libraries::scan(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int64_t libraryId)
{
    LOG_INFO<<std::format("User-Agent {}", req->getHeader("User-Agent"));
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    orm::Mapper<models::Libraries> mp(dbClientPtr);
    models::Libraries library{};
    try
    {
        library = mp.findByPrimaryKey(libraryId);
    }
    catch (drogon::orm::UnexpectedRows e) 
    { 
        LOG_WARN<<e.what();
        std::cerr << "error:" << e.what() << std::endl;
        callback(errorResponse("Не корректный id библиотеки", ErrorCode::IncorectData, HttpStatusCode::k400BadRequest));
        //return;
    }
    // threadPool.enqueue_detach(&scanLibrary, library, ScanMode::Full);
    threadPool.detach_task([library]{
        scanLibrary(std::move(library), ScanMode::Full);
    });
    //scanLibrary(library,ScanMode::Full);
    Json::Value ret;
    ret["result"] = "ok";
    auto resp = HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
    return;
}

void Libraries::scanAll(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback)
{
    
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    orm::Mapper<models::Libraries> mp(dbClientPtr);
    std::vector<models::Libraries> libraries = mp.findAll();
    for (auto& library : libraries)
    {
        threadPool.detach_task([&library]{
            scanLibrary(library, ScanMode::Full);
        });
    }
    auto resp = HttpResponse::newHttpResponse();
    callback(resp);
    return;
}

void Libraries::libraryPath(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int64_t libraryId, const int64_t libraryPathId)
{
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    std::shared_ptr<orm::Transaction> transaction = dbClientPtr->newTransaction();
    orm::Mapper<models::LibraryPaths> mp(transaction);
    Json::Value ret;
    ret["result"] = "ok";
    size_t deletitionCount{};
    // Хотя мы можем словить ошибку БД если у нас что то будет с подключением
    try 
    {
        deletitionCount = mp.deleteByPrimaryKey(libraryId);
    } 
    catch (drogon::orm::DrogonDbException e) 
    {
        std::cerr << "error:" << e.base().what() << std::endl;
        transaction->rollback();
        callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    if (deletitionCount == 0)
    {
        resp->setStatusCode(HttpStatusCode::k204NoContent);
        callback(resp);
    }
    else
    {
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
    }
    return;
}
void Libraries::libraryPaths(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int64_t libraryId)
{
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    orm::Mapper<models::LibraryPaths> mp(dbClientPtr);
    std::vector<models::LibraryPaths> library_paths;
    Json::Value ret = Json::arrayValue;
    // ret[0];
    // ret.clear();
    try 
    {
        library_paths = mp.findAll();
    }
    catch (drogon::orm::DrogonDbException e) 
    {
        std::cerr << "error:" << e.base().what() << std::endl;
        callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
    //libraries = mp.findBy(drogon::orm::Criteria(models::Users::Cols::_id, orm::CompareOperator::In, tmp));
    Json::Value pathJson;
    for (auto path : library_paths)
    {
        pathJson = path.toJson();
        //pathJson.removeMember("library_id");
        ret.append(pathJson);
    }
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(HttpStatusCode::k200OK);
    callback(resp);
    return;
}
void Libraries::createLibraryPath(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int64_t libraryId)
{
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    std::shared_ptr<orm::Transaction> transaction = dbClientPtr->newTransaction();
    orm::Mapper<models::LibraryPaths> mp(transaction);
    orm::Mapper<models::Libraries> mpLibrary(dbClientPtr);
    Json::Value ret;
    std::string err;
    Json::Value reqJson = *req->getJsonObject();
    //reqJson["id"] = utils::getUuid();
    if (!reqJson["paths"].isArray())
    {
        callback(errorResponse("Paths должны быть array"));
        return;
    }
    if (reqJson["paths"].size() == 0)
    {
        callback(errorResponse("Нет данных"));
        return;
    }
    try
    {
        mpLibrary.findByPrimaryKey(libraryId);
    }
    catch (drogon::orm::UnexpectedRows e) {
        LOG_WARN<<e.what();
        std::cerr << "error:" << e.what() << std::endl;
        callback(errorResponse("Не корректный id библиотеки", ErrorCode::IncorectData, HttpStatusCode::k400BadRequest));
        return;
    }
    Json::Value pathJson{};
    
    pathJson["library_id"] = libraryId;
    try 
    {
        std::string sPath;
        ret["errorPath"];
        Json::Value errorPath;
        for (auto path : reqJson["paths"])
        {
            sPath = path.asString();
            if (!pathExists(sPath))
            {
                LOG_DEBUG<<"ERROR";
                ret["errorPath"].append(sPath);
                continue;
            }
            //pathJson["id"]=utils::getUuid();
            // dbUtils insert
            pathJson["path"]=sPath;
            models::LibraryPaths pathModel(pathJson);
            mp.insert(pathModel);
            ret["path"].append(pathJson);
        }
    } 
    catch (drogon::orm::UniqueViolation e) { // сделаем пустой обработчик для UniqueViolation (пусть только логирует)
        LOG_WARN<<e.what();
        std::cerr << "error:" << e.what() << std::endl;
        callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
    catch (drogon::orm::UnexpectedRows e)
    {
        LOG_ERROR<<e.what();
        transaction->rollback();
        std::cerr << "error:" << e.what() << std::endl;
        callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
    ret["result"] = "ok";
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(HttpStatusCode::k200OK);
    callback(resp);
    return;
}
void Libraries::updateLibraryPath(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int64_t libraryId, const int64_t libraryPathId)
{
    LOG_DEBUG<<"libraryId "<<libraryId<<" libraryPathId "<<libraryPathId;
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    std::shared_ptr<orm::Transaction> transaction = dbClientPtr->newTransaction();
    orm::Mapper<models::LibraryPaths> mp(dbClientPtr);
    Json::Value ret;
    Json::Value reqJson = *req->getJsonObject();
    if (!reqJson.isMember("path"))
    {
        callback(errorResponse("Путь отсутствует"));
        return;
    }
    else
    {
        if (reqJson["path"].asString().size()==0)
        {
            callback(errorResponse("Пустой путь"));
            return;
        }
    }
    size_t deletitionCount{};
    // Хотя мы можем словить ошибку БД если у нас что то будет с подключением
    models::Libraries library;
    models::LibraryPaths path;
    std::future<size_t> updateFuture;
    try 
    {
        LOG_DEBUG<<"sadsa";
        path = mp.findByPrimaryKey(libraryPathId);
        library = path.getLibraries(transaction);
        if (*library.getId() != libraryId)
        {
            callback(errorResponse("Путь с данным id принадлежит другой библиотеки", ErrorCode::IncorectData, HttpStatusCode::k500InternalServerError));
            return;
        }
        std::string sPath = reqJson["path"].asString();
        if (sPath == *path.getPath())
        {
            ret["path"] = path.toJson();
            ret["result"]="ok";
            auto resp=HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
            return;
        }
        if (!pathExists(sPath))
        {
            callback(errorResponse("Не существующий путь"));
            return;
        }
        path.setPath(sPath);
        updateFuture = threadPool.submit_task([&mp, &path](){
            return mp.update(path);
        });
    } 
    catch (drogon::orm::UniqueViolation e) { // сделаем пустой обработчик для UniqueViolation (пусть только логирует)
        LOG_WARN<<e.what();
        std::cerr << "error:" << e.what() << std::endl;
        callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
    catch (drogon::orm::DrogonDbException e) 
    {
        std::cerr << "error:" << e.base().what() << std::endl;
        transaction->rollback();
        callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
    catch (...) 
    {
        std::cerr << "Необработаное исключение";
        transaction->rollback();
        callback(errorResponse("Необработаное исключение", ErrorCode::IncorectData, HttpStatusCode::k500InternalServerError));
        return;
    }
    if (updateFuture.get()==0)
    {
        callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));\
        return;
    }
    ret["path"] = path.toJson();
    ret["result"]="ok";
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
    return;
}
void Libraries::deleteLibraryPath(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int64_t libraryId, const int64_t libraryPathId)
{
    orm::DbClientPtr dbClientPtr = drogon::app().getDbClient();
    std::shared_ptr<orm::Transaction> transaction = dbClientPtr->newTransaction();
    orm::Mapper<models::LibraryPaths> mp(transaction);
    Json::Value ret;
    //ret["result"] = "ok";
    size_t deletitionCount{};
    // Хотя мы можем словить ошибку БД если у нас что то будет с подключением
    models::Libraries library;
    models::LibraryPaths path;
    try 
    {
        path = mp.findByPrimaryKey(libraryPathId);
        library = path.getLibraries(transaction);
        if (*library.getId() != libraryId)
        {
            callback(errorResponse("Путь с данным id принадлежи другой библиотеки", ErrorCode::IncorectData, HttpStatusCode::k500InternalServerError));
            return;
        }
        deletitionCount = mp.deleteByPrimaryKey(libraryPathId);
    } 
    catch (drogon::orm::DrogonDbException e) 
    {
        std::cerr << "error:" << e.base().what() << std::endl;
        transaction->rollback();
        callback(errorResponse("Database error", ErrorCode::DataBaseError, HttpStatusCode::k500InternalServerError));
        return;
    }
    ret["result"]="ok";
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    if (deletitionCount == 0)
    {
        resp->setStatusCode(HttpStatusCode::k204NoContent);
        callback(resp);
    }
    else
    {
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
    }
    return;
}