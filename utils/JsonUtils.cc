#include "JsonUtils.h"
#include "DBUtils.h"
#include "Libraries.h"
#include "LibraryPaths.h"
#include "MediaEnums.h"
#include "MediaItems.h"
#include "MediaUtils.h"
#include "Utils.h"
#include "coro/sync_wait.hpp"
#include <cstdint>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpResponse.h>
#include <drogon/orm/DbClient.h>
#include <filesystem>
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


coro::task<Json::Value> libraryToJson(const models::Libraries& library)
{
    orm::DbClientPtr dbPtr = app().getDbClient();
    Json::Value ret = library.toJson();
    Json::Value& pathsJson = (ret["paths"] = Json::arrayValue);
    auto paths = library.getLibraryPaths(dbPtr);
    for (const auto& path : paths)
    {
        pathsJson.append(Json::Value{std::move(path.getValueOfPath())});
    }   
    co_return ret;
}

coro::task<Json::Value> libraryToJson(const int64_t libraryID)
{
    auto library = coro::sync_wait(findRecordByPrimaryKeyORM<models::Libraries>(libraryID));
    if (!library)
        co_return {};
    co_return co_await libraryToJson(*library);
}


coro::task<Json::Value> mediaItemToJson(const int64_t mediaItemID, const Language language)
{
    auto mediaItem = coro::sync_wait(findRecordByPrimaryKeyORM<models::MediaItems>(mediaItemID));
    if (!mediaItem)
        co_return {};
    co_return co_await mediaItemToJson(*mediaItem, language);
}

coro::task<Json::Value> mediaItemToJson(const models::MediaItems& mediaItem, const Language language)
{
    Json::Value ret{};
    int64_t mediaItemID = mediaItem.getPrimaryKey();
    ret["id"] = mediaItemID;
    ret["parentID"] = (mediaItem.getValueOfParentId() == 0 ? Json::nullValue : mediaItem.getValueOfParentId());
    ret["season"] = mediaItem.getValueOfEpisode();
    ret["episode"] = mediaItem.getValueOfEpisode();
    std::string title, description, tagline;
    co_await getMediaItemTitle(mediaItemID, language, title, description, tagline);
    ret["title"] = std::move(title);
    ret["description"] = std::move(description);
    ret["tagline"] = std::move(tagline);
    ret["type"] = mediaItem.getValueOfMediaItemTypeId(); 
    ret["path"] = mediaItem.getValueOfPath();

    Json::Value& streams = (ret["streams"] = Json::arrayValue);
    auto dbPointer = app().getDbClient();
    auto s = mediaItem.getMediaItemStreams(dbPointer);
    for (auto& stream : s)
        streams.append(mediaStreamToJson(stream));
    
    Json::Value& images = (ret["images"] = Json::arrayValue);
    auto imagesORM = co_await getImagesForMediaItem(mediaItemID, language, Language::en);
    for (auto& image : imagesORM)
        images.append(imageToJson(image));

    Json::Value& credits = (ret["credits"] = Json::arrayValue);
    auto creditsORM = co_await getCreditsForMediaItemID(mediaItemID);
    for (auto& credit : creditsORM)
        credits.append(creditToJson(credit));

    Json::Value& extIDs = (ret["externalIDs"] = Json::arrayValue);
    auto extIDsORM = mediaItem.getExternalMediaItemIds(dbPointer);
    for (const auto& extID : extIDsORM)
        extIDs.append(externalMediaItemIDToJson(extID));
    
    // На будущее
    // Json::Value& userData = (ret["userData"] = Json::objectValue);
    co_return ret;
}

Json::Value mediaStreamToJson(const models::MediaItemStreams& stream)
{
    Json::Value ret{};
    ret["name"] = std::move(stream.getValueOfName());
    ret["streamIndex"] = stream.getValueOfStreamIndex();
    ret["type"] = stream.getValueOfStreamTypeId();
    ret["bitrate"] = stream.getValueOfBitrate();
    ret["width"] = stream.getValueOfWidth();
    ret["height"] = stream.getValueOfHeight();
    return ret;
}

Json::Value imageToJson(const models::Images& image)
{
    Json::Value ret{};
    ret["fileName"] = fs::path(image.getValueOfPath()).filename().string();
    ret["imageType"] = image.getValueOfImageTypeId();
    ret["language"] = image.getValueOfLanguageId();
    ret["origin"] = image.getValueOfOrigin();
    return ret;
}

Json::Value creditToJson(const models::Credits& credit)
{
    Json::Value ret;
    ret["id"] = credit.getPrimaryKey();
    ret["type"] = credit.getValueOfCreditTypeId();
    ret["personID"] = credit.getValueOfPersonId();
    return ret;
}

Json::Value externalMediaItemIDToJson(const models::ExternalMediaItemIds& extId)
{
    Json::Value ret{};
    ret["origin"] = extId.getValueOfMetadataProviderId();
    ret["extID"] = extId.getValueOfExternalId();
    return ret;
}