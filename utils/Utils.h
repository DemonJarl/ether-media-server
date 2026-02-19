#pragma once
#include "coro/task.hpp"
#include "cpr/api.h"
#include "cpr/cprtypes.h"
#include "cpr/response.h"
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <filesystem>
#include <string>
#include <cpr/cpr.h>
#include "boost/algorithm/string.hpp"
#include "cpr/timeout.h"

using namespace drogon; 
namespace fs = std::filesystem;

inline bool pathExists(const std::string& path)
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

inline bool pathExists(const std::filesystem::path& path)
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
inline bool createDirectoryIfNotExists(const std::string& path)
{
    if (pathExists(path))
        return true;
    std::error_code ec;
    fs::create_directory(path);
    if (ec)
    {
        LOG_ERROR << ec.message();
    }
    return true;

}
inline bool isStringEmptyCopy(const std::string& str)
{
    return boost::trim_copy(str).size() == 0;
}

inline bool isStringEmpty(std::string& str)
{
    boost::trim(str);
    return str.size() == 0;
}

template <typename... Ts>
inline coro::task<cpr::Response> getCoro(Ts&&... ts)
{
    
    co_return cpr::Get(ts...);
} 