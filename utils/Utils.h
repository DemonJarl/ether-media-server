#pragma once
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <filesystem>
#include <string>
#include "boost/algorithm/string.hpp"
using namespace drogon; 
bool pathExists(const std::string& path);
bool pathExists(const std::filesystem::path& path);
inline bool isStringEmptyCopy(const std::string& str)
{
    return boost::trim_copy(str).size() == 0;
}

inline bool isStringEmpty(std::string& str)
{
    boost::trim(str);
    return str.size() == 0;
}