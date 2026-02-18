#include "Models/MovieCastCredit.hpp"

namespace TMDBAPI::Models 
{
    MovieCastCredit::MovieCastCredit(const Json::Value &json) : CastCredit(json)
    {
        if (!json.find("cast_id") || json["cast_id"].isNull())
            throw std::invalid_argument("");
        castId = json["cast_id"].asInt();
    }
}