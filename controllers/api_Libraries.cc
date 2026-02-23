#include "api_Libraries.h"
#include "DBUtils.h"
#include "Libraries.h"
#include "LibraryPaths.h"
#include "LibrarySettingsManager.hpp"
#include "MediaEnums.h"
#include "MediaItems.h"
#include "MediaUtils.h"
#include "ThreadPool.h"
#include "Users.h"
#include "coro/sync_wait.hpp"
#include "coro/task.hpp"
#include "utils/JsonUtils.h"
#include "utils/Utils.h"
#include <cstdint>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon_test.h>
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
#include <optional>
#include <queue>
#include <string>
#include <sys/types.h>
#include <trantor/utils/Logger.h>
#include <vector>
//extern dp::thread_pool threadPool;
// перейти на uuid?
using namespace api;
namespace models = drogon_model::sqlite3;



void Libraries::getMediaItems(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int64_t libraryId)
{
    //drogon::utils::splitString(const std::string &str, const std::string &separator)
    if (!coro::sync_wait(recordExists<models::Libraries>(orm::Criteria(models::Libraries::Cols::_id, orm::CompareOperator::EQ, libraryId))))
    {
        callback(errorResponse("", ErrorCode::ObjectNotFound, drogon::k404NotFound));
        return;
    }
    Json::Value ret{};
    Json::Value& jsonArr = (ret["items"] = Json::arrayValue);

    std::vector<models::MediaItems> mediaItems = coro::sync_wait(getMediaItemsByLibrary(libraryId));
    if (mediaItems.empty())
    {
        auto resp=HttpResponse::newHttpJsonResponse(std::move(ret));
        callback(resp);
        return;
    }
    ret["itemCount"] = mediaItems.size();
    for (const auto& item : mediaItems)
    {
        jsonArr.append(coro::sync_wait(mediaItemToJson(item, Language::ru)));
    }
    auto resp=HttpResponse::newHttpJsonResponse(std::move(ret));
    callback(resp);
    return;
    //findRecordsByCriteriaORM<>(orm::Criteria(models::Libraries::Cols::_id, orm::CompareOperator::EQ, libraryId))
}

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
    Json::Value& reqJson = *req->getJsonObject();
    
    // Сделать функцию для валидации настроек библиотеки
    if (reqJson["settings"].empty() || !reqJson["settings"].isObject())
        reqJson["settings"] = LibrarySettingsManager::defaultLibrarySetting().toStyledString();

    if (!models::Libraries::validateJsonForCreation(reqJson, err))
    {
        callback(errorResponse(err, ErrorCode::DataBaseError));
        return;
    }
    models::Libraries library(reqJson);
    if (!coro::sync_wait(insertRecord(library)))
    {
        callback(errorResponse(err, ErrorCode::IncorectData, HttpStatusCode::k422UnprocessableEntity));
        return;
    }
    if (findAndCheckNullArray(reqJson, "paths"))
    {
        int64_t libraryId = library.getPrimaryKey();
        for (const auto& path : reqJson["paths"])
        {
            if (!path.is<std::string>())
                continue;
            std::string p = path.asString();
            if (!pathExists(p))
                continue;
            coro::sync_wait(insertPath(libraryId, p));
        }
    }
    ret = coro::sync_wait(libraryToJson(library));
    ret["result"] = "ok";
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
    Json::Value& reqJson = *req->getJsonObject();
    if (reqJson.empty())
    {
        callback(errorResponse("", ErrorCode::EmptyData, drogon::k400BadRequest));
        return;
    }
    auto library = coro::sync_wait(findRecordByPrimaryKeyORM<models::Libraries>(libraryId));
    if (!library)
    {
        callback(errorResponse("", ErrorCode::ObjectNotFound, drogon::k404NotFound));
        return;
    }
    Json::Value ret{};
    bool libraryModified;
    if (findAndCheckNullAndType<std::string>(reqJson, "name"))
    {
        libraryModified = true;
        (*library).setName(reqJson["name"].asString());
    }
    if (findAndCheckNullAndType<std::string>(reqJson, "settings"))
    {
        libraryModified = true;
        (*library).setSettings(reqJson["settings"].asString());
    }
    if (libraryModified)
    {
        if (!coro::sync_wait(updateRecord(*library)))
        {
            callback(errorResponse("", ErrorCode::IncorectData, HttpStatusCode::k422UnprocessableEntity));
            return;
        }
    }

    if (findAndCheckNullArray(reqJson, "paths"))
    {
        coro::sync_wait(deletePaths(libraryId));
        coro::sync_wait(clearLibraryMediaItemsAssignments(libraryId));
        for (const auto& path : reqJson["paths"])
        {
            if (!path.is<std::string>())
                continue;
            std::string p = path.asString();
            if (!pathExists(p))
                continue;
            coro::sync_wait(insertPath(libraryId, p));
        }
        coro::sync_wait(assignMediaItemsByLibrary(libraryId));
        coro::sync_wait(deleteUnAssignedMediaItems());
    }
    ret = coro::sync_wait(libraryToJson(*library));
    callback(HttpResponse::newHttpJsonResponse(ret));
    return;
}

void Libraries::scan(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback,
            const int64_t libraryId)
{
    LOG_INFO<<std::format("User-Agent {}", req->getHeader("User-Agent"));
    std::optional<models::Libraries> library{};
    library = coro::sync_wait(findRecordByPrimaryKeyORM<models::Libraries>(libraryId));
    if (!library)
    {
        callback(errorResponse("Нет такой библиотеки", ErrorCode::EmptyData, HttpStatusCode::k404NotFound));
        return;
    }
    // threadPool.enqueue_detach(&scanLibrary, library, ScanMode::Full);
    threadPool.detach_task([library = std::move(*library)]{
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
    scanLibraries(ScanMode::Full);
    Json::Value ret;
    ret["result"] = "ok";
    auto resp = HttpResponse::newHttpJsonResponse(ret);
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