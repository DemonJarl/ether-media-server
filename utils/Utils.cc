#include "Utils.h"
#include <drogon/HttpResponse.h>
#include <filesystem>
#include <memory>
#include <system_error>
#include <trantor/utils/Logger.h>

namespace fs = std::filesystem;

bool pathExists(const std::string& path)
{
    LOG_DEBUG<<"Проверка пути "<<path; 
    std::error_code ec;
    bool result = fs::exists(path, ec);
    if (ec)
    {
        LOG_ERROR << ec.message();
    }
    return result;
}

bool pathExists(const std::filesystem::path& path)
{
    LOG_DEBUG<<"Проверка пути "<<path.string(); 
    std::error_code ec;
    bool result = fs::exists(path, ec);
    if (ec)
    {
        LOG_ERROR << ec.message();
    }
    return result;
}
