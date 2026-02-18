#pragma once
#include "Credits.h"
#include <cstddef>
#include <cstdint>
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>
#include <models/MediaItems.h>
#include <optional>
#include <type_traits>
#include <vector>

using namespace drogon;
namespace models = drogon_model::sqlite3;

//models::MediaItems::PrimaryKeyType

template<typename model>
concept ModelORMWithPK = requires (model) {
    model::primaryKeyName;
};

template<ModelORMWithPK model, typename KeyType>
std::optional<model> findRecordByPrimaryKeyORM(const KeyType& key, orm::DbClientPtr dbPointer = nullptr)
{
    static_assert(std::is_same<typename model::PrimaryKeyType, KeyType>(), "Типа ключа должен быть равным типу ключа таблицы");
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<model> mp(dbPointer);
    try
    {
        return mp.findByPrimaryKey(key);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
    return {};
}

template<ModelORMWithPK model, typename KeyType>
std::optional<KeyType> findRecordByPrimaryKey(const KeyType& key, orm::DbClientPtr dbPointer = nullptr)
{
    static_assert(std::is_same<typename model::PrimaryKeyType, KeyType>(), "Типа ключа должен быть равным типу ключа таблицы");
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<model> mp(dbPointer);
    return findRecordByPrimaryKeyORM<model>(key).getPrimaryKey();
}

template<ModelORMWithPK model>
std::optional<model> findRecordByCriteriaORM(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr) 
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    orm::Mapper<model> mp(dbPointer);
    try
    {
        return mp.findOne(criteria);   
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
}

template<ModelORMWithPK model>
std::optional<typename model::PrimaryKeyType> findRecordByCriteria(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    auto ret = findRecordByCriteriaORM<model>(criteria, dbPointer);
    if (ret.has_value())
        return std::make_optional(ret->getPrimaryKey());
    return {};
}

template<ModelORMWithPK model>
std::vector<model> findRecordsByCriteriaORM(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr) 
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    orm::Mapper<model> mp(dbPointer);
    
    try
    {
        return std::move(mp.findBy(criteria));
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
}

template<ModelORMWithPK model>
std::vector<typename model::PrimaryKeyType> findRecordsByCriteria(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    auto records = findRecordsByCriteriaORM<model>(criteria, dbPointer);
    if (records.empty())
        return {};
    std::vector<typename model::PrimaryKeyType> ret{records.size()};
    for (const auto& record : records)
        ret.push_back(record.getPrimaryKey());
    return ret;
}

template<ModelORMWithPK model>
bool recordExists(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    return findRecordByCriteriaORM<model>(criteria, dbPointer).has_value();
}

template<ModelORMWithPK model>
auto recordCount(orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    orm::Mapper<model> mp(dbPointer);
    return mp.count();
}

template<ModelORMWithPK model>
auto recordCount(const orm::Criteria& criteria, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();

    orm::Mapper<model> mp(dbPointer);
    return mp.count(criteria);
}

template<ModelORMWithPK model>
std::optional<typename model::PrimaryKeyType> insertRecord(model& record, orm::Mapper<model>& mp)
{
    try
    {
        mp.insert(record);
        return record.getPrimaryKey();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
}
// Для полноты
template<ModelORMWithPK model>
std::optional<typename model::PrimaryKeyType> insertRecord(model& record, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<model> mp(dbPointer);
    try
    {
        return insertRecord(record, mp);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
}

template<ModelORMWithPK model>
bool updateRecord(model& record, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<model> mp(dbPointer);
    try
    {
        mp.update(record);
        return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
}


template<ModelORMWithPK model>
bool updateRecord(model& record, orm::Mapper<model>& mp)
{
    try
    {
        mp.update(record);
        return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
}

template<ModelORMWithPK model, typename KeyType>
bool deleteRecordByPrimaryKey(const KeyType& key, const orm::Mapper<model>& mp)
{
    static_assert(std::is_same<typename model::PrimaryKeyType, KeyType>(), "Типа ключа должен быть равным типу ключа таблицы");
    try
    {
        mp.deleteByPrimaryKey(key);
        return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
}

template<ModelORMWithPK model, typename KeyType>
bool deleteRecordByPrimaryKey(const KeyType& key, orm::DbClientPtr dbPointer = nullptr)
{
    static_assert(std::is_same<typename model::PrimaryKeyType, KeyType>(), "Типа ключа должен быть равным типу ключа таблицы");
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<model> mp(dbPointer);

    try
    {
        mp.deleteByPrimaryKey(key);
        return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
}

template <typename ...Args>
bool execSQL(const std::string& sql, Args&&...args)
{
    auto dbPointer = app().getDbClient();
    try
    {
        dbPointer->execSqlSync(sql, args...);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
    return true;
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