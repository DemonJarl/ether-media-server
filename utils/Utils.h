#pragma once
#include "coro/task.hpp"
#include "cpr/api.h"
#include "cpr/cprtypes.h"
#include "cpr/response.h"
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <filesystem>
#include <shared_mutex>
#include <string>
#include <cpr/cpr.h>
#include <vector>
#include "boost/algorithm/string.hpp"
#include "cpr/timeout.h"

using namespace drogon; 
namespace fs = std::filesystem;

// template<typename T>
// class ThreadSafeVector
// {
// private:
//     std::vector<T> data;
//     mutable std::shared_mutex mtx;
// public:
//     size_t size() const
//     {
//         std::shared_lock lock(mtx);
//         return data.size();
//     }
//     T operator[](size_t ind)
//     {

//     }
// };



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
    fs::create_directory(path, ec);
    if (ec)
    {
        LOG_ERROR << ec.message();
        return false;
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

inline bool deleteFile(const std::string& path)
{
    std::error_code ec;
    // игнорируем вовзвращаемое значение потому что если файла нету то возвращает false
    fs::remove(path, ec);
    if (ec)
    {
        LOG_ERROR << ec.message();
        return false;
    }
    return true;
}

inline bool getAbsolutePath(std::string& path)
{
    LOG_INFO<<std::format("getAbsolutePath {}", path);
    if (!pathExists(path))
        return false;
    std::error_code ec;
    std::string ret = fs::absolute(path, ec);
    if (ec)
    {
        LOG_ERROR << ec.message();
        return false;
    }
    path = ret;
    // path = std::move(ret);
    return true;
}

template <typename... Ts>
inline coro::task<cpr::Response> getCoro(Ts&&... ts)
{
    co_return cpr::Get(ts...);
} 