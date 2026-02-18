#include "api_Enviroment.h"
#include <filesystem>
#include <json/value.h>
#include <optional>
#include <string>
#include <trantor/utils/Logger.h>
#include "utils/JsonUtils.h"
#include "filesystem"
using namespace api;
namespace fs = std::filesystem;
void Enviroment::drives(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback)
{
    Json::Value ret;
    std::string path = req->getParameter("path");
    // Надо на  винде посмотреть
     LOG_DEBUG<<path;
    for (const auto & entry : fs::directory_iterator(path)) 
    {
        LOG_DEBUG<< entry.path().c_str();
        ret.append(entry.path().c_str());
    }

    auto resp=HttpResponse::newHttpJsonResponse(NULL);
            resp->setStatusCode(HttpStatusCode::k200OK);
            callback(resp);
    return;
}

void Enviroment::directoryContents(const HttpRequestPtr &req,
            std::function<void (const HttpResponsePtr &)> &&callback)
{
    Json::Value ret;
    std::string path = req->getParameter("path");
    std::optional<bool> parIncludeDir = req->getOptionalParameter<bool>("includeDirectories").has_value();
    bool includeDir = (parIncludeDir.has_value()) ? parIncludeDir.value() : false;
    std::optional<bool> parIncludeFiles = req->getOptionalParameter<bool>("includeFiles").has_value();
    bool includeFiles = (parIncludeFiles.has_value()) ? parIncludeFiles.value() : false;

    LOG_DEBUG<<"includeFiles "<< includeFiles;
    LOG_DEBUG<<"includeDir "<< includeDir;
    if ((!includeFiles && !includeDir) || path.size() == 0)
    {
        callback(errorResponse("Неправильные параметры"));
        return;
    }
    for (const auto & entry : fs::directory_iterator(path)) 
    {

        LOG_DEBUG<< entry.path().c_str();
        if (entry.is_directory() && includeDir)
        {
            ret.append(file(entry.path().filename(), entry.path(), "Directory"));
            continue;
        }
        if (includeFiles)
            ret.append(file(entry.path().filename(), entry.path(), "File", entry.path().filename().extension()));
    }
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(HttpStatusCode::k200OK);
    callback(resp);
    return;
}
// Add definition of your processing function here
