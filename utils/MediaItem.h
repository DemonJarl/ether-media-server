#pragma once
#include <string>
#include "MediaEnums.h"
#include "MediaItemLocalizedText.h"
#include <models/MediaItems.h>
#include <models/SubtitleTracks.h>
#include <models/AudioTracks.h>
#include <models/GenreAssignments.h>
#include <vector>

namespace models = drogon_model::sqlite3;
// struct MediaItem
// {
//     std::string name{};
//     std::string path{}; // в виде строки потому что возможно перейдем на llfio
//     ItemType type{Unknown};
//     int season{-1};
//     int episode{-1};
//     MediaItem(std::string name, std::string path, ItemType type, int season, int episode);
//     MediaItem() : MediaItem("","", ItemType::Unknown, -1, -1){};
//     MediaItem(const MediaItem &) = delete;
//     MediaItem(MediaItem &&other);
//     MediaItem& operator=(MediaItem&& other);
//     MediaItem& operator=(MediaItem& other) = default;
//     bool operator!=(const MediaItem& other);
//     bool operator==(const MediaItem& other);

// };

struct MediaItem
{
    models::MediaItems item{};
    std::vector<models::SubtitleTracks> subtitlesTracks{};
    std::vector<models::MediaItemLocalizedText> titles{};
    std::vector<models::AudioTracks> audioTracks{};
    std::vector<models::GenreAssignments> genres{};
    MediaItem() = default;
    //MediaItem(models::MediaItems mediaItem, drogon::orm::DbClientPtr dbCon){}; // Написать конструктор 
};