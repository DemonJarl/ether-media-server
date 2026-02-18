#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <string>
#include <sys/types.h>

using namespace drogon;
namespace api
{
class Users : public drogon::HttpController<Users>
{
  static Json::Value createDefaultUserJson();
  static Json::Value createUserJson(uint64_t userID, std::string login);
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(Users::getUsers, "", Get, "AuthFilter");
    METHOD_ADD(Users::updateUser, "{id}", Put, "AuthFilter");
    METHOD_ADD(Users::getUser, "{id}", Get, "AuthFilter");
    METHOD_ADD(Users::getUser, "{id}/settings", Get, "AuthFilter");
    METHOD_ADD(Users::deleteUser, "", Delete, "AuthFilter");// Добавить AdminFilter
    METHOD_ADD(Users::me, "me", Get, Options, "AuthFilter");// Добавить AdminFilter
    METHOD_LIST_END
    void getUsers(const HttpRequestPtr &req,
                 std::function<void (const HttpResponsePtr &)> &&callback);
    void getUser(const HttpRequestPtr &req,
                 std::function<void (const HttpResponsePtr &)> &&callback,
                 std::string userId);
    void getUserSettings(const HttpRequestPtr &req,
                 std::function<void (const HttpResponsePtr &)> &&callback);
    void deleteUser(const HttpRequestPtr &req,
                 std::function<void (const HttpResponsePtr &)> &&callback);
    void updateUser(const HttpRequestPtr &req,
                 std::function<void (const HttpResponsePtr &)> &&callback,
                 std::string userId);
    void me(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback);
};
}
