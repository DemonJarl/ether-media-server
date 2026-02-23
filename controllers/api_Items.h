#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api
{
class Items : public drogon::HttpController<Items>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(Items::get, "/{2}/{1}", Get); // path is /api/Items/{arg2}/{arg1}
    // METHOD_ADD(Items::your_method_name, "/{1}/{2}/list", Get); // path is /api/Items/{arg1}/{arg2}/list
    METHOD_ADD(Items::getChildren, "/{id}/getChildren", Get, Options, "AuthFilter"); 
    METHOD_ADD(Items::getParent, "/{id}/getParent", Get, Options, "AuthFilter");

    METHOD_LIST_END

    void getChildren(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback, const int64_t mediaItemID);

    void getParent(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback, const int64_t mediaItemID);
};
}