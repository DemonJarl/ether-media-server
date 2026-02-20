#include <drogon/HttpAppFramework.h>
#include <drogon/drogon.h>
#include <trantor/utils/Logger.h>
#include "Enums/Enums.hpp"
#include "ServerSettingsManager.hpp"
#include "ThreadPool.h"
#include "MetaDataAPI/TMDB/BaseAPI.hpp"
#include "DBUtils.h"
int main() {
    LOG_DEBUG<<"SPD "<<trantor::Logger::hasSpdLogSupport();
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", 5555);
    //Load config file
    drogon::app().loadConfigFile("../config.json");
    drogon::app().registerPostHandlingAdvice(
        [](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp) {
            //LOG_DEBUG << "postHandling1";
            resp->addHeader("Access-Control-Allow-Origin", "*");
            req->addHeader("Access-Control-Allow-Origin", "*");
        });
    // Функция запускается сражу же после запуска сервера
    drogon::app().registerBeginningAdvice([]{
        ServerSettingsManager& serverSettings = ServerSettingsManager::Instance();
        TMDBAPI::BaseAPI::setLanguage(TMDBAPI::Languages::Ru);
        TMDBAPI::BaseAPI::setKey(serverSettings.getTMDBKey());
    });
    drogon::app().setIntSignalHandler([]{
        LOG_INFO<<"Сервер выключается";
        threadPool.purge();
        metaDataThreadPool.purge();
        scanThreadPool.purge();
        threadPool.wait();
        LOG_INFO<<"Начинаем ожидание scanThreadPool";
        scanThreadPool.wait();
        LOG_INFO<<"Начинаем ожидание metaDataThreadPool";
        metaDataThreadPool.wait();
        app().quit();
    });
   
    //drogon::app().loadConfigFile("../config.yaml");
    //Run HTTP framework,the method will block in the internal event loop
    //threadPool.Start();
    LOG_DEBUG<<"ThreadPool "<<threadPool.get_thread_count();
    drogon::app().run();
    //app().getDbClient()->setTimeout(double timeout)
    return 0;
}
