#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api
{
class Enviroment : public drogon::HttpController<Enviroment>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(Enviroment::get, "/{2}/{1}", Get); // path is /api/Enviroment/{arg2}/{arg1}
    // METHOD_ADD(Enviroment::your_method_name, "/{1}/{2}/list", Get); // path is /api/Enviroment/{arg1}/{arg2}/list
    // ADD_METHOD_TO(Enviroment::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
    METHOD_ADD(Enviroment::drives, "/drives", Get, "AuthFilter");
    METHOD_ADD(Enviroment::directoryContents, "/directoryContents", Get, "AuthFilter");
    METHOD_LIST_END
    void drives(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback);
    void directoryContents(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback);
    
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
}
