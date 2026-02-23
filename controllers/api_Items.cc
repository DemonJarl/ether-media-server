#include "api_Items.h"
#include "DBUtils.h"
#include "JsonUtils.h"
#include "MediaEnums.h"
#include "MediaItems.h"
#include "coro/sync_wait.hpp"
#include <drogon/HttpResponse.h>
#include <drogon/orm/Criteria.h>
#include <json/value.h>

using namespace api;

void Items::getChildren(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback, const int64_t mediaItemID)
{
    Language language = Language::ru;
    auto mediaItem = coro::sync_wait(findRecordByPrimaryKeyORM<models::MediaItems>(mediaItemID));
    if (!mediaItem)
    {
        callback(HttpResponse::newNotFoundResponse());
        return;
    }
    auto items = (*mediaItem).getMediaItems(app().getDbClient());
    
    Json::Value ret{};
    Json::Value& jsonArr = (ret["items"] = Json::arrayValue);
    ret["itemCount"] = items.size();
    for (const auto& item : items)
        jsonArr.append(coro::sync_wait(mediaItemToJson(item, language)));

    callback(HttpResponse::newHttpJsonResponse(ret));
    return;
}

void Items::getParent(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback, const int64_t mediaItemID)
{
    
    Language language = Language::ru;
    auto mediaItem = coro::sync_wait(findRecordByPrimaryKeyORM<models::MediaItems>(mediaItemID));
    if (!mediaItem)
    {
        callback(HttpResponse::newNotFoundResponse());
        return;
    }
    auto parentMediaItem = coro::sync_wait(findRecordByPrimaryKeyORM<models::MediaItems>((*mediaItem).getValueOfParentId()));
    auto items = (*mediaItem).getMediaItems(app().getDbClient());
    
    Json::Value ret = coro::sync_wait(mediaItemToJson(*parentMediaItem, language));
    
    callback(HttpResponse::newHttpJsonResponse(ret));
    return;
}
// Add definition of your processing function here
