#pragma once
#include <cstdint>
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <drogon/RequestStream.h>
#include <json/value.h>
#include <string>
#include <trantor/utils/Logger.h>
#include <models/Sessions.h>
#include <models/Users.h>


using namespace drogon;

void updateSessionLastAccess(int64_t sessionID, orm::DbClientPtr dbPointer = nullptr);
void deleteSession(int64_t sessionID, orm::DbClientPtr dbPointer = nullptr);
std::string createSession(int64_t userID, const std::string& device, orm::DbClientPtr dbPointer = nullptr);
bool isSessionValid(const std::string& authToken, orm::DbClientPtr dbPointer = nullptr);