#include "EventUtils.h"
#include "TimeUtils.h"
#include "Events.h"
#include "Sessions.h"
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Mapper.h>
#include <type_traits>

namespace models = drogon_model::sqlite3;

bool isDiscriminatedRecordValid(TableDiscriminator tableDiscriminator, int64_t record_id)
{
    return false;
}

bool createEvent(EventType type, int64_t user_id, orm::DbClientPtr dbPointer, TableDiscriminator tableDiscriminator, int64_t record_id)
{
    if (!dbPointer)
       dbPointer = drogon::app().getDbClient();
    orm::Mapper<models::Events> mp(dbPointer);
    models::Events event;
    event.setEventTypeId(static_cast<std::underlying_type<EventType>::type>(type));
    event.setUserId(user_id);
    if (tableDiscriminator != TableDiscriminator::None)
    {
        if (!isDiscriminatedRecordValid(tableDiscriminator, record_id))
            return false;
        event.setTableDescrimintator(static_cast<std::underlying_type<TableDiscriminator>::type>(tableDiscriminator));
        event.setRecordId(record_id);
    }
    event.setTimestamp(getCurrentDateTime());
    return true;
}