#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

using namespace drogon;

namespace api
{
class Auth : public drogon::HttpController<Auth>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(Auth::get, "/{2}/{1}", Get); // path is /api/Auth/{arg2}/{arg1}
    // METHOD_ADD(Auth::your_method_name, "/{1}/{2}/list", Get); // path is /api/Auth/{arg1}/{arg2}/list
    // ADD_METHOD_TO(Auth::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
    METHOD_ADD(Auth::logout, "/logout", Post, Options, "AuthFilter");
    METHOD_ADD(Auth::login, "/login", Post, Options);
    METHOD_ADD(Auth::me, "/me", Get, Options, "AuthFilter");
    METHOD_ADD(Auth::signUp, "signUp", Post, Options);
    METHOD_LIST_END
    void logout(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback);
    void login(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback);
    void me(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback);
    void signUp(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback);
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
}
