#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class Images : public drogon::HttpController<Images>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(Images::get, "/{2}/{1}", Get); // path is /Images/{arg2}/{arg1}
    // METHOD_ADD(Images::your_method_name, "/{1}/{2}/list", Get); // path is /Images/{arg1}/{arg2}/list
    // ADD_METHOD_TO(Images::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_ADD(Images::getImage, "/{}", Get, Options);
    METHOD_LIST_END
    void getImage(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback, const std::string& imageFileName);
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
