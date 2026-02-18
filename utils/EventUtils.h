#pragma once
#include <cstdint>
#include <models/EventTypes.h>
#include <models/Events.h>

using namespace drogon;

enum class EventType : uint8_t
{
    LogIn = 0,
    LogOut,
    SignUp,
    StartedWatching,
    FinishedWatching,
    MarkedAsFinished
};

enum class TableDiscriminator : uint8_t
{
    None,
    MediaItems,
    Users
};

bool isDiscriminatedRecordValid(TableDiscriminator tableDiscriminator, int64_t record_id);

bool createEvent(EventType type, int64_t user_id, orm::DbClientPtr dbPointer = nullptr, TableDiscriminator tableDiscriminator = TableDiscriminator::None, int64_t record_id = 0);