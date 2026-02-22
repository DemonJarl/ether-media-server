#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <string>
//constinit dp::thread_pool threadPool;
using namespace drogon;
// Подумать над regex
// Какие то проблемы с ним
const std::string UUID_REGEX = "^[0-9(a-f|A-F)]{8}-[0-9(a-f|A-F)]{4}-4[0-9(a-f|A-F)]{3}-[89ab][0-9(a-f|A-F)]{3}-[0-9(a-f|A-F)]{12}$";
namespace api
{

class Libraries : public drogon::HttpController<Libraries>
{
  public:
    METHOD_LIST_BEGIN
    // REST API
    METHOD_ADD(Libraries::getLibrary, "/{id}", Get,Options, "AuthFilter"); 
    METHOD_ADD(Libraries::getLibraries, "", Get,Options, "AuthFilter"); // список библиотек
    METHOD_ADD(Libraries::createLibrary, "", Post,Options, "AuthFilter");
    METHOD_ADD(Libraries::deleteLibrary, "/{id}", Delete,Options, "AuthFilter");
    METHOD_ADD(Libraries::modifyLibrary, "/{id}", Patch,Options, "AuthFilter");

    METHOD_ADD(Libraries::libraryPath, "/{id}/paths/{path_id}", Get,Options, "AuthFilter");
    METHOD_ADD(Libraries::updateLibraryPath, "/{id}/paths/{path_id}", Put,Options, "AuthFilter");
    METHOD_ADD(Libraries::deleteLibraryPath, "/{id}/paths/{path_id}", Delete,Options, "AuthFilter");
    METHOD_ADD(Libraries::libraryPaths, "/{id}/paths", Get,Put,Options, "AuthFilter");
    METHOD_ADD(Libraries::createLibraryPath, "/{id}/paths", Post,Options, "AuthFilter");

    //METHOD_ADD(Libraries::modifyLibrary, "/{id}/path/{path_id}", Put,Options, "AuthFilter");
    // RPC API
    METHOD_ADD(Libraries::scan, "/{id}/scan", Post,Options, "AuthFilter");
    METHOD_ADD(Libraries::scanAll, "/scan", Post,Options, "AuthFilter");
    METHOD_LIST_END
    // надо наверное будет сгруппировать функции
    void getLibraries(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback);
    void scan(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback,
               const int64_t libraryId);
    void scanAll(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback);
    void getLibrary(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback,
               const int libraryId);
    void createLibrary(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback);
    void deleteLibrary(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback,
               const int64_t libraryId);
    void modifyLibrary(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback,
               const int64_t libraryId);
    void libraryPath(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback,
               const int64_t libraryId, const int64_t libraryPathId);
    void updateLibraryPath(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback,
               const int64_t libraryId, const int64_t libraryPathId);
    void deleteLibraryPath(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback,
               const int64_t libraryId, const int64_t libraryPathId);
    void libraryPaths(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback,
               const int64_t libraryId);
    void createLibraryPath(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback,
               const int64_t libraryId);
};
}
