#include "SessionUtils.h"
#include "TimeUtils.h"
#include "Sessions.h"
#include <drogon/orm/Criteria.h>
#include <drogon/utils/Utilities.h>
#include "EventUtils.h"

// Добавить создание событий

namespace models = drogon_model::sqlite3;

std::string createSession(int64_t userID, const std::string& device, orm::DbClientPtr dbPointer)
{
    if (!dbPointer)
       dbPointer = drogon::app().getDbClient();

    orm::Mapper<models::Sessions> mp(dbPointer);
    models::Sessions session;
    session.setDevice(device);
    session.setUserId(userID);
    session.setToken(utils::getUuid());
    session.setLastAccess(getCurrentDateTime());
    try 
    {
        mp.insert(session);
        // createEvent(EventType::SignUp, userID, dbPointer);
        return std::move(*session.getToken());
    } 
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return "";
    }
}

void deleteSession(int64_t sessionID, orm::DbClientPtr dbPointer)
{
    if (!dbPointer)
       dbPointer = drogon::app().getDbClient();

    orm::Mapper<models::Sessions> mp(dbPointer);
    try 
    {
        mp.deleteByPrimaryKey(sessionID);
    } 
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
    }
}

void updateSessionLastAccess(int64_t sessionID, orm::DbClientPtr dbPointer)
{
    if (!dbPointer)
       dbPointer = drogon::app().getDbClient();

    orm::Mapper<models::Sessions> mp(dbPointer);
    try 
    {
        models::Sessions session = mp.findByPrimaryKey(sessionID);
        session.setLastAccess(getCurrentDateTime());
        mp.update(session);
    } 
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
    }
}

bool isSessionValid(const std::string& authToken, orm::DbClientPtr dbPointer)
{
    if (authToken.length() != 36)
        return false;

    if (!dbPointer)
       dbPointer = drogon::app().getDbClient();
 
    
    orm::Mapper<models::Sessions> mp(dbPointer);
    try
    {
        models::Sessions session = mp.findOne(orm::Criteria(models::Sessions::Cols::_token, orm::CompareOperator::EQ, authToken));
        session.setLastAccess(getCurrentDateTime());
        mp.update(session);
        
    } 
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
    return true;
}