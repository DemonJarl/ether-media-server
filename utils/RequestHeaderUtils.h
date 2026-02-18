#pragma once
#include <drogon/utils/Utilities.h>
#include <string>
#include <trantor/utils/Logger.h>
#include "Macros.h"

enum class DeviceIdentificationMode
{
};

FLAG_ENUM(DeviceIdentificationMode);

inline std::string getDevice(const std::string& userAgent)
{
    std::string ret;
    ret = userAgent.substr(0, userAgent.find("/")-1);
    LOG_DEBUG<<"User Agent "<<userAgent;
    return ret;
}

inline std::string getAuthToken(const std::string& bearer)
{
    // LOG_DEBUG<<bearer<<" "<<bearer.length();
    if (bearer.length() != 43)
        return "";
    return bearer.substr(7);
}