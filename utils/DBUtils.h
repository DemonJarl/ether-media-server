#pragma once
#include "Credits.h"
#include <cstddef>
#include <cstdint>
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/Result.h>
#include <models/MediaItems.h>
#include <optional>
#include <type_traits>
#include <vector>
#include "coro/task.hpp"
#include <coro/coro.hpp>

using namespace drogon;
namespace models = drogon_model::sqlite3;

//models::MediaItems::PrimaryKeyType

template<typename model>
concept ModelORMWithPK = requires (model) {
    model::primaryKeyName;
};

template<ModelORMWithPK model, typename KeyType>
coro::task<std::optional<model>> findRecordByPrimaryKeyORM(const KeyType& key, orm::DbClientPtr dbPointer = nullptr)
{
    static_assert(std::is_same<typename model::PrimaryKeyType, KeyType>(), "Типа ключа должен быть равным типу ключа таблицы");
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<model> mp(dbPointer);
    try
    {
        co_return mp.findByPrimaryKey(key);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return {};
    }
    co_return {};
}

template<ModelORMWithPK model, typename KeyType>
coro::task<std::optional<KeyType>> findRecordByPrimaryKey(const KeyType& key, orm::DbClientPtr dbPointer = nullptr)
{
    static_assert(std::is_same<typename model::PrimaryKeyType, KeyType>(), "Типа ключа должен быть равным типу ключа таблицы");
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<model> mp(dbPointer);
    co_return findRecordByPrimaryKeyORM<model>(key).getPrimaryKey();
}

template<ModelORMWithPK model>
coro::task<std::optional<model>> findRecordByCriteriaORM(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr) 
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    orm::Mapper<model> mp(dbPointer);
    try
    {
        co_return mp.findOne(criteria);   
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return {};
    }
}

template<ModelORMWithPK model>
coro::task<std::optional<typename model::PrimaryKeyType>> findRecordByCriteria(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    auto ret = co_await findRecordByCriteriaORM<model>(criteria, dbPointer);
    if (ret.has_value())
        co_return std::make_optional(ret->getPrimaryKey());
    co_return {};
}

template<ModelORMWithPK model>
coro::task<std::vector<model>> findRecordsByCriteriaORM(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr) 
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    orm::Mapper<model> mp(dbPointer);
    
    try
    {
        co_return std::move(mp.findBy(criteria));
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return {};
    }
}

template<ModelORMWithPK model>
coro::task<std::vector<typename model::PrimaryKeyType>> findRecordsByCriteria(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    auto records = findRecordsByCriteriaORM<model>(criteria, dbPointer);
    if (records.empty())
        co_return {};
    std::vector<typename model::PrimaryKeyType> ret{records.size()};
    for (const auto& record : records)
        ret.push_back(record.getPrimaryKey());
    co_return ret;
}

template<ModelORMWithPK model>
coro::task<bool> recordExists(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    co_return (co_await findRecordByCriteriaORM<model>(criteria, dbPointer)).has_value();
}

template<ModelORMWithPK model>
coro::task<size_t> recordCount(orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    orm::Mapper<model> mp(dbPointer);
    co_return mp.count();
}

template<ModelORMWithPK model>
coro::task<size_t> recordCount(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    orm::Mapper<model> mp(dbPointer);
    co_return mp.count(criteria);
}

template<ModelORMWithPK model>
coro::task<std::optional<typename model::PrimaryKeyType>> insertRecord(model& record, orm::Mapper<model>& mp)
{
    try
    {
        mp.insert(record);
        co_return record.getPrimaryKey();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return {};
    }
}
// Для полноты
template<ModelORMWithPK model>
coro::task<std::optional<typename model::PrimaryKeyType>> insertRecord(model& record, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<model> mp(dbPointer);
    try
    {
        co_return (co_await insertRecord(record, mp));
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return {};
    }
}

template<ModelORMWithPK model>
coro::task<bool> updateRecord(model& record, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<model> mp(dbPointer);
    try
    {
        mp.update(record);
        co_return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return false;
    }
}


template<ModelORMWithPK model>
coro::task<bool> updateRecord(model& record, orm::Mapper<model>& mp)
{
    try
    {
        mp.update(record);
        co_return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return false;
    }
}

template<ModelORMWithPK model, typename KeyType>
coro::task<bool> deleteRecordByPrimaryKey(const KeyType& key, const orm::Mapper<model>& mp)
{
    static_assert(std::is_same<typename model::PrimaryKeyType, KeyType>(), "Типа ключа должен быть равным типу ключа таблицы");
    try
    {
        mp.deleteByPrimaryKey(key);
        co_return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return false;
    }
}

template<ModelORMWithPK model, typename KeyType>
coro::task<bool> deleteRecordByPrimaryKey(const KeyType& key, orm::DbClientPtr dbPointer = nullptr)
{
    static_assert(std::is_same<typename model::PrimaryKeyType, KeyType>(), "Типа ключа должен быть равным типу ключа таблицы");
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<model> mp(dbPointer);

    try
    {
        mp.deleteByPrimaryKey(key);
        co_return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return false;
    }
}

template <typename ...Args>
coro::task<std::optional<orm::Result>> execSQL(orm::DbClientPtr dbPointer, const std::string& sql, Args&&...args)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    try
    {
        co_return co_await dbPointer->execSqlCoro(sql, args...);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return {};
    }
    co_return {};
}
// template <typename T, typename ...Args>
// std::optional<T> execSQLSingle(const std::string& sql, const std::string& resultColumnName, Args&&...args)
// {
//     auto dbPointer = app().getDbClient();
//     try
//     {
//         auto res = dbPointer->execSqlSync(sql, args...);
        
//         return res[0][resultColumnName].template as<T>();
//     }
//     catch (const drogon::orm::DrogonDbException& e)
//     {
//         LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
//         return false;
//     }
//     return true;
// }