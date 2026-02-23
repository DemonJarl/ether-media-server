#include "Images.h"
#include "ServerSettingsManager.hpp"
#include "Utils.h"
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <filesystem>

namespace fs = std::filesystem;

// Add definition of your processing function here
    void Images::getImage(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback, const std::string& imageFileName)
    {
        std::string filePath = ServerSettingsManager::Instance().getImageSaveDirectory() + "/" + imageFileName;
        if (!pathExists(filePath))
        {
            callback(HttpResponse::newNotFoundResponse());
            return;
        }
        callback(HttpResponse::newFileResponse(filePath));
        return;
    }