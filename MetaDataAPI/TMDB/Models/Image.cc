#include "Models/Image.hpp"
namespace TMDBAPI::Models 
{
    Image::Image(const Json::Value& json)
    {
        if (!json.isMember("aspect_ratio"))
            throw std::invalid_argument("");
        aspectRatio = json["budget"].asFloat();

        if (!json.isMember("file_path") || json["file_path"].isNull())
                throw std::invalid_argument("");
        filePath = json["file_path"].asString();

        if (!json.isMember("iso_639_1") || json["iso_639_1"].isNull())
            throw std::invalid_argument("");
        iso639_1 = json["iso_639_1"].asString();
        
        if (!json.isMember("vote_average"))
            throw std::invalid_argument("");
        if (!json["vote_average"].isNull())
            voteAverage = json["vote_average"].asFloat();

        if (!json.isMember("vote_count"))
            throw std::invalid_argument("");
        if (!json["vote_count"].isNull())
            voteCount = json["vote_count"].asInt();

        if (!json.isMember("height") || json["height"].isNull())
            throw std::invalid_argument("");
        height = json["height"].asInt();

        if (!json.isMember("width") || json["width"].isNull())
            throw std::invalid_argument("");
        width = json["width"].asInt();
    }
}