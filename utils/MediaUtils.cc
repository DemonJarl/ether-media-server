#include "MediaUtils.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <drogon/HttpTypes.h>
#include <drogon/drogon_test.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Exception.h>
#include <drogon/orm/Result.h>
#include <exception>
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Mapper.h>
#include <filesystem>
#include <json/value.h>
#include <future>
#include <iterator>
#include <map>
#include <models/Libraries.h>
#include <models/LibraryPaths.h>
#include <models/MediaItems.h>
#include <mutex>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <trantor/utils/LockFreeQueue.h>
#include <trantor/utils/Logger.h>
#include <string_view>
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include <array>
#include "BS_thread_pool.hpp"
#include "BaseAPI.hpp"
#include "CastCredit.hpp"
#include "CreditAssignments.h"
#include "CreditLocalizations.h"
#include "Credits.h"
#include "CrewCredit.hpp"
#include "DBUtils.h"
#include "Endpoints/TVSeries.hpp"
#include "Enums/Enums.hpp"
#include "ExternalMediaItemIds.h"
#include "ExternalPeopleIds.h"
#include "GenreAssignments.h"
#include "GenreLocalizations.h"
#include "Genres.h"
#include "Images.h"
#include "Languages.h"
#include "MediaEnums.h"
#include "MediaItemImageAssignments.h"
#include "MediaItemLocalizations.h"
#include "MediaItemStreams.h"
#include "Models/TVSeries.hpp"
#include "MovieExternalIds.hpp"
#include "PartialMediaImages.hpp"
#include "People.h"
#include "People.hpp"
#include "PersonExternalIds.hpp"
#include "ProductionCompanies.h"
#include "ProductionCompanyAssignments.h"
#include "PeopleLocalizations.h"
#include "TVEpisode.hpp"
#include "TVEpisodeExternalIds.hpp"
#include "TVSeason.hpp"
#include "TVSeriesExternalIds.hpp"
#include "ThreadPool.h"
#include "Utils.h"
#include "caches/cache.hpp"
#include "caches/fifo_cache_policy.hpp"
#include "caches/lru_cache_policy.hpp"
#include "EnumToInt.h"
#include "MetaDataAPI/TMDB/Endpoints/SearchTV.hpp"
#include "MetaDataAPI/TMDB/Models/TVSeries.hpp"
#include "MetaDataAPI/TMDB/Endpoints/SearchMovie.hpp"
#include "MetaDataAPI/TMDB/Endpoints/TVSeason.hpp"
#include "MetaDataAPI/TMDB/Endpoints/TVEpisode.hpp"
#include "MetaDataAPI/TMDB/Endpoints/Movie.hpp"
#include "cpr/cprtypes.h"
#include <format>
extern "C" 
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}
#include "cpr/cpr.h"

using namespace drogon;
namespace models = drogon_model::sqlite3;
namespace fs = std::filesystem;

// Видео форматы для фильтрации
const std::array<std::string, 75> videoFileExtensions{".m4b",".qcif",".mpo",".3g2",".apng",".f4b",".h263",".vc1",".f4a",".mj2",
    ".roq",".m4v",".mkv",".f4p",".mvi",".psp",".avi",".dv",".ogv",".asf",".mpg",".ogg",".f4v",".mod",
    ".m4p",".mng",".qt",".webm",".dif",".mjpg",".m4a",".yuv10",".wmv",".flv",".h261",".cgi",".j2k",
    ".gifv",".rm",".mjpeg",".vob",".h264",".xl",".mpv",".svi",".avs",".avs2",".mp4",".obu",".nsv",
    ".ismv",".h26l",".hevc",".yuv",".amv",".cdxl",".idf",".3gp",".mpeg",".mov",".gif",".h265",".mks",
    ".mp2",".mpe",".rrc",".ism",".cif",".c2",".avc",".mk3d",".rgb",".v210",".isma",".mxf"}; // ,".mka"

const std::vector<std::string>titleMetaDataKeys{"title","name","show", "showName"};
const std::vector<std::string> seasonInfoMetaDataKeys{"season_num","seasonNumber"};
const std::vector<std::string> episodeNameMetaDataKeys{"episode_id"};
const std::vector<std::string> episodeNumMetaDataKeys{"episode_number","episode_sort","episode_id","episodeNumber"};

const std::regex episodeSeasonPattern("(S[0-9]{1,2})(E[0-9]{1,2})|((S[0-9]{1,2})|(E[0-9]{1,2}))", std::regex_constants::icase);
const std::regex isSeasonEpisodePattern("(- ?[0-9]{1,3} ?-?)|\\b(S[0-9]{1,2})(E[0-9]{1,2})|((\\bS[0-9]{1,2})|(\\bE[0-9]{1,2}))|\\b(0[0-9])", std::regex_constants::icase);

template <typename Key, typename Value>
using lru_cache_t = typename caches::fixed_sized_cache<Key, Value, caches::FIFOCachePolicy>;

// Потестировать создание прогресс бара через websockets на nuxt


std::mutex getParentMutex, findOrInsertPersonMutex, findOrInsertCreditMutex;
std::counting_semaphore sem(60);
std::array<char, 4> badSymbols{'-', '.', ',', '|'};

bool isNumber(const std::string &s) 
{
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void cleanupFileName(std::string& fileName)
{
    fileName = std::regex_replace(fileName, std::regex("\\[(.*?)\\]"), "");
    fileName = std::regex_replace(fileName, std::regex("[\\(\\)\\[\\]]"), "");
    std::smatch match;
    std::regex_search(fileName, match, std::regex("\\b[0-9]{4}[ip]", std::regex_constants::icase));
    if (!match.empty())
        fileName = fileName.substr(0, fileName.find(match.str()) - 1);
    // Подчисть "-","." в конце пример "Dimension 20 - "
    boost::trim(fileName);
    auto len = fileName.size();
    // LOG_DEBUG<<std::format("filename {}, last char {}", fileName, fileName.at(len-1));
    // if (isElementInContainer(fileName.at(len-1), badSymbols))
    // {
    //     fileName = fileName.substr(0, len-2);
    //     LOG_DEBUG<<std::format("filename2 {}", fileName);
    //     boost::trim(fileName);
    // }
}

std::string cleanupFileNameCopy(const std::string& fileName)
{
    std::string ret = fileName;
    cleanupFileName(ret);
    return ret;
}

int strToInt(const std::string& str)
{
    if(str.empty())
        return 0;
    std::string parsedString;
    parsedString.reserve(str.size());
    for (const auto& c : str)
    {
        if (!isdigit(c))
            continue;
        parsedString += c;
    }
    if (parsedString.empty())
        return 0;
    return std::stoi(parsedString);
}

int strToInt(const char* str)
{
    if(str == nullptr)
        return 0;
    std::string parsedString;
    while (str != NULL)
    {
        if (!isdigit(*str))
            continue;
        parsedString += *str;
        str++;
    }
    if (parsedString.empty())
        return 0;
    return std::stoi(parsedString);
}

bool getSeasonEpisode(const std::string& str, int& season, int& episode)
{
    // std::cout<<"getSeasonEpisode("<<str<<")\n";
    season = episode = 0;
    std::smatch matchResult;
    if (std::regex_match(str, episodeSeasonPattern))
    {
        std::string::const_iterator searchStart( str.cbegin() );
        while ( std::regex_search( searchStart, str.cend(), matchResult, std::regex{"[SE][0-9]{1,2}", std::regex_constants::icase}) )
        {
            std::string s = std::move(matchResult[0].str()); 
            switch (tolower(s[0])) 
            {
                case 'e':
                    episode = std::atoi(&s[1]);
                    break;
                case 's':
                    season = std::atoi(&s[1]);
                    break;
            }
            searchStart = matchResult.suffix().first;
        }
    }
    else 
        episode = strToInt(str);

    if (season == 0 && episode > 0)
        season = 1;
    
    return episode > 0 || season > 0;
}

std::string splitStrToStr(const std::vector<std::string>& splitStr, std::string sep = "", std::vector<std::string>::size_type start = 0, std::vector<std::string>::size_type end = 0)
{
    std::string ret;
    if (end == 0)
        end = splitStr.size();

    if (start >= end)
        throw new std::invalid_argument("start должен быть меньше чем end");

    for (auto i = start; i < end; ++i)
    {
        ret += (!ret.empty() ? sep : "") + std::move(splitStr[i]);
    }
    return ret;
}

std::string splitStrToStrCopy(const std::vector<std::string>& splitStr,  std::string sep = "", std::vector<std::string>::size_type start = 0, std::vector<std::string>::size_type end = 0)
{
    std::string ret;
    if (end == 0)
        end = splitStr.size();

    if (start >= end)
        throw std::invalid_argument("start должен быть меньше чем end");

    for (auto i = start; i < end; ++i)
    {
        ret += (!ret.empty() ? sep : "") + splitStr[i];
    }
    return ret;
}

void scanLibraries(ScanMode mode, orm::DbClientPtr dbPointer)
{
    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();
    orm::Mapper<models::Libraries> mp(dbPointer);
    
    try 
    {
        std::vector<models::Libraries> libraries =  mp.findAll();
        threadPool.detach_loop(0, libraries.size(), [&libraries, &mode](const std::size_t i)
        {
            scanLibrary(libraries[i], mode);
        });
        return;
    }  
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return;
    }
}

void scanLibrary(const int64_t libraryID, ScanMode mode, orm::DbClientPtr dbPointer)
{
    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();
    orm::Mapper<models::Libraries> mp(dbPointer);
    try 
    {
        scanLibrary(mp.findByPrimaryKey(libraryID), mode, dbPointer);
        return;
    }  
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return;
    }

    return;
}
// Библиотеки имеют персональные настройки
// В отделбьной функции их легче обрабатывать
// Можно сделать глобальную переменую чтобы выдавать ошибку ИЛИ сделать это в callback для прогресса
// Наверное стоит сделать callback для результатов и WebSocket апдейта
void scanLibrary(const models::Libraries& library, ScanMode mode, orm::DbClientPtr dbPointer)
{

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();

    execSQL("PRAGMA journal_mode=WAL");
    execSQL("PRAGMA busy_timeout=5000");
    //std::shared_ptr<orm::Transaction> transaction = dbClientPtr->newTransaction();
    //orm::Mapper<models::Libraries> mp(transaction);
    std::vector<models::LibraryPaths> paths{};
    paths = library.getLibraryPaths(dbPointer);
    size_t pathCount = paths.size();
    //LOG_DEBUG<<"Получено "<<pathCount<<" путей";
    if (pathCount == 0)
    {
        LOG_DEBUG<<"Получено 0 путей";
        return;
    }
    // Проверяем пути
    // orm::Mapper<models::LibraryPaths> mapperPaths(dbPointer);
    // orm::Mapper<models::MediaItems> mapperMedia(dbPointer);
    // std::vector<std::future<size_t>> pathDeletionFutures;
    // std::vector<std::future<std::vector<models::MediaItems>>> mediaFutures{pathCount};
    // std::vector<models::MediaItems> mediaItems{};
    // LOG_DEBUG<<"AAA??? ";
    // std::erase_if(paths, [&mapperPaths, &pathDeletionFutures, &mediaFutures, &dbPointer](models::LibraryPaths& path)->bool
    // {
    //     if (!pathExists(*path.getPath()))
    //     {
    //         threadPool.detach_task([&path, &mapperPaths]{mapperPaths.deleteByPrimaryKey(path.getPrimaryKey());});
    //         return true;
    //     }
    //     // Переписать
    //     // mediaFutures.push_back(threadPool.submit_task([&path]{
    //     //     return SQLMapperGetMediaItemsByPath(path.getValueOfPath());
    //     // }));
    //     //mediaFutures.push_back(std::async(std::launch::async, &models::LibraryPaths::getMediaItems, &path, dbClientPtr));
    //     return false;
    // });

    // std::for_each(
    //     std::execution::par_unseq, std::begin(mediaFutures), std::end(mediaFutures), [&mediaItems](std::future<std::vector<models::MediaItems>>& future){
    //         try 
    //         {
    //             std::vector<models::MediaItems> tmpVec =  future.get();
    //             const size_t vecSize = tmpVec.size();
    //             if (vecSize > 0)
    //             {
    //                 //models::MediaItems tmpItem = tmpVec[0];
    //                 LOG_DEBUG<<"По пути "<<*tmpVec[0].getId()<<" получено "<<vecSize<<" медиа предметов";
    //                 mediaItems.insert(mediaItems.end(), std::move_iterator(tmpVec.begin()), std::move_iterator(tmpVec.end()));
    //             }
    //         } 
    //         catch (orm::UnexpectedRows e) 
    //         {
    //             LOG_ERROR << "error:" << e.what();
    //         }
    //     });
    // mediaFutures.clear();
    // std::for_each(
    //     std::execution::par_unseq, std::begin(pathDeletionFutures), std::end(pathDeletionFutures), [](std::future<size_t>& future){
    //         try 
    //         {
    //             LOG_DEBUG<<"Удалено "<<future.get()<<" путей";
    //         } 
    //         catch (orm::UnexpectedRows e) 
    //         {
    //             LOG_ERROR << "error:" << e.what();
    //         }
    //     });
    // pathDeletionFutures.clear();
    // pathCount = paths.size(); 
    // LOG_DEBUG<<"После фильтрации осталось "<<pathCount<<" путей";
    // if (pathCount == 0)
    //     return;
    // // Получаем уже существующие медиа файлы
    // size_t mediaCount = mediaItems.size();
    // LOG_DEBUG<<"Получено медиафайлов: "<<mediaCount;
    // LOG_DEBUG<<library.getValueOfSettings();
    // Почему при вызове конструктора он умирает
    //LibraryScanSettings scanSettings{};
    LibraryScanSettings scanSettings(library);
    // LOG_ERROR<<"AAA!!!!!!!1111!";
    // const LibraryScanSettings scanSettings{};
    // std::vector<std::string> preExistingPaths{mediaCount};
    LOG_DEBUG<<std::format("scanSettings.MetaDataProvider: {}", enumToInt(scanSettings.metaDataProvider));
    std::vector<std::string> preExistingPaths;


    // //  анализ полученых медиа файлов
    // std::for_each(
    //     std::execution::par_unseq, std::begin(mediaItems), std::end(mediaItems), [&preExistingPaths](models::MediaItems& mediaItem){
    //         preExistingPaths.emplace_back(mediaItem.getValueOfPath());
    //     });

    // if (mode == ScanMode::MetaDataOnly)
    // {
    //     return;
    // }
    // std::vector<MediaItem> items{};
    std::vector<std::future<void>> scanFutures;

    for (auto& path : paths)
    {
        sem.acquire();
        //LOG_DEBUG<<"AAA!!!!!!!! "<<path.getValueOfPath();
        scanFutures.emplace_back(scanThreadPool.submit_task([&preExistingPaths, &scanSettings, &path](){
            scanPath(*path.getPath(), preExistingPaths, scanSettings, *path.getId());
        }));
        for (auto& future : scanFutures)
        {
            future.wait();
        }
        scanFutures.clear();
        sem.release();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    LOG_ERROR<<std::format("Закончили сканирование за {} сек", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000000.0);
    return;
}
void analyzeFile(const fs::directory_entry& file, const LibraryScanSettings& scanSettings, const int64_t pathId)
{
    models::MediaItems item;
    auto path = file.path();
    LOG_DEBUG<<"Начали анализ файла "<<file.path();
    AVFormatContext *fmt_ctx = NULL;
    std::string title{},parsedSeriesTitle{};
    int episode{}, season{}, year{};
    int retAv;
    if ((retAv = avformat_open_input(&fmt_ctx, path.c_str(), NULL, NULL)))
    {
        LOG_ERROR<<"Не удалось открыть файл по пути "<<path;
        return;
    }
    if (path.empty()) 
        item.setPathToNull();
    else
        item.setPath(path);
    parseFileName(path.stem(), parsedSeriesTitle, season, episode, year);
    orm::Mapper<models::MediaItems> mp(app().getDbClient());
    if (episode != 0)
    {
        item.setEpisode(episode);
        item.setSeason(season);
        item.setMediaItemTypeId(enumToInt(MediaType::TVEpisode));
        item.setParsedName(std::format("{} S{}E{}",parsedSeriesTitle, season, episode));
        std::optional<int64_t> parentID = getParentForEpisode(parsedSeriesTitle, season, "", scanSettings);
        if (!parentID.has_value())
        {
            LOG_ERROR<<std::format("Ошибка нахождения родителя для эпизода ТВ шоу {}", parsedSeriesTitle);
            avformat_close_input(&fmt_ctx); // важно!
            return;
        }
        item.setParentId(*parentID);
        item.setLastUpdated(getCurrentDateTime());
        try
        {
            mp.insert(item);
        }
        catch (const drogon::orm::DrogonDbException& e)
        {
            LOG_ERROR<<std::format("Ошибка вставки media_items: {} {}", e.base().what(), item.getValueOfPath());
            avformat_close_input(&fmt_ctx); // важно!
            return;
        }
        getTVEpisodeMetaData(scanSettings, item);
    }
    else 
    {
        item.setEpisodeToNull();
        item.setSeasonToNull();
        item.setMediaItemTypeId(enumToInt(MediaType::Movie));
        item.setParsedName(parsedSeriesTitle);
        item.setLastUpdated(getCurrentDateTime());
        try
        {
            mp.insert(item);
        }
        catch (const drogon::orm::DrogonDbException& e)
        {
            LOG_ERROR<<std::format("Ошибка вставки media_items: {} {}", e.base().what(), item.getValueOfPath());
            avformat_close_input(&fmt_ctx); // важно!
            return;
        }
        getMovieMetaData(scanSettings, item);
        // просто получить информацию о фильме
    }
    insertStreams(fmt_ctx->streams, fmt_ctx->nb_streams, item.getPrimaryKey());
    assignMediaItemToLibraryByPath(item.getPrimaryKey(), path);
    avformat_close_input(&fmt_ctx); // важно!
    return;
}


void assignMediaItemToLibraryByPath(const int64_t mediaItemID, const std::string& path)
{   
    try
    {
        auto res = drogon::app().getDbClient()->execSqlSync("select library_paths.library_id from library_paths where instr($1, library_paths.path) = 1", path);
        if (res.size() == 0)
            return;
        for (auto id : res)
            assignMediaItemToLibraryByID(mediaItemID, id["library_id"].as<int64_t>());
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка назначения библиотеки по пути: "<<e.base().what();
        return;
    }
}

void assignMediaItemToLibraryByID(const int64_t mediaItemID, const int64_t libraryID)
{
    try
    {
        auto res = drogon::app().getDbClient()->execSqlSync("insert into media_item_library_assignments (media_item_id, library_id) values($1,$2)", mediaItemID, libraryID);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка назначения библиотеки по ID: "<<e.base().what();
        return;
    }
}


void scanPath(const fs::path& path, const std::vector<std::string>& scanedPaths, const LibraryScanSettings& scanSettings, const int64_t pathId)
{
    LOG_DEBUG<<"scanPath "<<path.string();
    std::vector<std::future<void>> analyzisFutures{};
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator{path, std::filesystem::directory_options::skip_permission_denied})
    {
        if (entry.is_directory())
            continue;
        //LOG_DEBUG <<"Сканирование директории";
        const auto filePath = entry.path();
        if(isElementInContainer(filePath.string(), scanedPaths) || !isElementInContainer(filePath.extension().string(), videoFileExtensions))
        {
            continue;   
        }
        LOG_DEBUG<<std::format("Сканирование директории {}", filePath.string());
        analyzisFutures.emplace_back(threadPool.submit_task([entry, &scanSettings, pathId](){
            analyzeFile(entry, scanSettings, pathId);
        }));
        // analyzeFile(entry, scanSettings, pathId);
        // break;
    }
    LOG_DEBUG<<std::format("Папок {}", analyzisFutures.size());
    for (auto& future : analyzisFutures)
    {
        future.wait();
    }
    return;
}

std::vector<fs::directory_entry> enumerateFiles(const std::string& path)
{
    std::vector<fs::directory_entry> ret;
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator{path, std::filesystem::directory_options::skip_permission_denied})
    {
        if (!entry.is_directory() && isElementInContainer(entry.path().extension().string(), videoFileExtensions))
        {
            ret.push_back(entry);
        }
    }
    return std::move(ret);
}

std::vector<fs::directory_entry> enumerateFiles(const std::vector<std::string>& paths)
{
    std::vector<fs::directory_entry> ret;
    for (const auto& path : paths)
    {
        auto tmp = enumerateFiles(path);
        ret.reserve(ret.size()+tmp.size());
        ret.insert(ret.end(), std::move_iterator(tmp.begin()), std::move_iterator(tmp.end()));
    }    
    return std::move(ret);
}

void insertStreams(AVStream** streams, uint streamCount, int64_t mediaItemID, orm::DbClientPtr dbPointer)
{
    if (streams == NULL || streamCount == 0)
        return;

    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();

    orm::Mapper<models::MediaItemStreams> mp(dbPointer);
    
    for (uint i = 0; i < streamCount; ++i)
    {
        if (!streams[i] || AVMediaTypeToStreamType( streams[i]->codecpar->codec_type) == StreamType::Unknown)
            continue;
        try 
        {
            models::MediaItemStreams stream = parseStream(*streams[i], mediaItemID);
            mp.insert(stream);
        } 
        catch (const drogon::orm::DrogonDbException& e)
        {
            LOG_ERROR<<"Ошибка вставки потока: "<<e.base().what();
            continue;
        }
    }
}

models::MediaItemStreams parseStream(AVStream& stream, int64_t mediaItemID)
{
    models::MediaItemStreams ret;
    AVCodecParameters* codecpar = stream.codecpar;

    StreamType streamType = AVMediaTypeToStreamType(codecpar->codec_type);

    if (streamType == StreamType::Unknown)
        throw std::invalid_argument("Данный тип кодека не возможно спарсить");

    ret.setMediaItemId(mediaItemID);
    ret.setLanguageId(enumToInt(Language::xx));
    ret.setStreamIndex(stream.index);
    ret.setStreamTypeId(enumToInt(streamType));
    ret.setDuration(stream.duration);
    const AVDictionaryEntry *tag = NULL;
    if ((tag = av_dict_get(stream.metadata, "title", nullptr, 0)))
        ret.setName(tag->value);
    else
        ret.setName("");
    if ((tag = av_dict_get(stream.metadata, "language", nullptr, 0)))
        ret.setLanguageId(enumToInt(Language::xx));
    switch (streamType) 
    {
        case StreamType::Video: 
            ret.setBitrate(codecpar->bit_rate);
            ret.setHeight(codecpar->height);
            ret.setWidth(codecpar->width);
            break;
        case StreamType::Audio:
            ret.setBitrate(codecpar->sample_rate);
            break;
        case StreamType::Subtitles:
            if ((tag = av_dict_get(stream.metadata, "BPS", nullptr, 0)))
                ret.setBitrate(std::atoi(tag->value));
            break;
        case StreamType::Unknown:
            throw std::invalid_argument("Тип потока должен быть аудио, видео или субтитры");
    }
    return ret;
}

bool getDataFromMetadataProviders(const LibraryScanSettings& scanSettings, const models::MediaItems& mediaItem)
{
    MediaType type = static_cast<MediaType>(mediaItem.getValueOfMediaItemTypeId());
    bool parsed = false;
    if (hasFlag(scanSettings.metaDataProvider, MetaDataProvider::TMDB))
    {
        // TMDBAPI::Endpoints::SearchTV
    }
    return false;
}


std::optional<int64_t> findSeasonMediaItemID(const std::string& seriesTitle, const int64_t season, const std::string& releaseDate, orm::DbClientPtr dbPointer)
{
    LOG_DEBUG<<std::format("findSeasonMediaItemID({}, {}, {})", seriesTitle, season, releaseDate);
    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();
    orm::Result result{nullptr};
    try
    {
        result = dbPointer->execSqlSync(
            "select season.id as id from media_items as media_item\
            left join media_item_localizations as loc_text on media_item_id = media_item.id\
            inner join media_items as season on ( coalesce(season.season,0) = $1 and season.parent_id = media_item.id)\
             where \
            (\
            media_item.media_item_type_id = "+std::to_string(enumToInt(MediaType::TVShow))+
            " and\
            (coalesce(loc_text.name,'') = $2 or media_item.parsed_name = $2)\
            and\
            (coalesce(media_item.release_date,'') = $3 or $3 = '') \
            ) limit 1", season, seriesTitle, releaseDate
        );
        if (result.empty())
        {
            LOG_DEBUG<<"Не удалось найти сезон";
            return {};
        }
        return result[0]["id"].as<int64_t>();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
    return {};
}

std::optional<int64_t> findShowMediaItemID(const std::string& seriesTitle, const std::string& releaseDate, orm::DbClientPtr dbPointer)
{
    LOG_ERROR<<std::format("findShowMediaItemID({}, {})", seriesTitle, releaseDate);

    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();
    orm::Result result{nullptr};
    try
    {
        result = dbPointer->execSqlSync(
            "select media_item.id as id from media_items as media_item\
            left join media_item_localizations as loc_text on media_item_id = media_item.id\
             where \
            (\
            media_item.media_item_type_id = "+std::to_string(enumToInt(MediaType::TVShow))+
            " and \
            (coalesce(loc_text.name,'') = $1 or media_item.parsed_name = $1)\
            and\
            (coalesce(media_item.release_date,'') = $2 or $2 = '')\
            ) limit 1",seriesTitle, releaseDate
        );
        if (result.empty())
        {
            LOG_ERROR<<std::format("Не нашли ТВ Шоу {}({})", seriesTitle, releaseDate);
            return {};
        }
        return result[0]["id"].as<int64_t>();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
    return {};
}

std::optional<int64_t> findShowByExternalID(const std::vector<std::pair<std::string, MetaDataProvider>> exteranlIDs)
{
    return {};
}

std::optional<int64_t> findSeasonByExternalID(const std::vector<std::pair<std::string, MetaDataProvider>> exteranlIDs)
{
    return {};
}

// подумать что делать если несколько копий одной серии
std::optional<int64_t> getParentForEpisode(const std::string& showTitle, const int season, const std::string& releaseDate, const LibraryScanSettings& scanSettings, orm::DbClientPtr dbPointer)
{
    std::lock_guard lockMutex(getParentMutex);
    LOG_ERROR<<std::format("getParentForEpisode({}, {}, {})", showTitle, season, releaseDate);
    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();
    if (season <= 0)
        std::invalid_argument("Сезон должен быть больше нуля");
    std::optional<int64_t> id;
    id = findSeasonMediaItemID(showTitle, season, releaseDate, dbPointer);
    if (id.has_value())
    {
        LOG_DEBUG<<std::format("Нашли родителя сезон({})[{}] для {}", season, *id, showTitle);
        return id;
    }
    // если нету ищем шоу и создаем шоу
    id = findShowMediaItemID(showTitle, releaseDate);
    if (id.has_value())
    {
        id = createTVSeasonMediaItem(scanSettings, *id, season, releaseDate);
        if (!id.has_value())
        {
            LOG_DEBUG<<std::format("Не смогли создать новый сезон для найденого сериала");
            return {};
        }
        // getTVSeasonMetaData(scanSettings, *id, getMediaItemExternalID(*id));
        return id;
    }
    // если нету то нужно создать и шоу и сезон
    auto showID = createTVShowMediaItem(scanSettings, showTitle, releaseDate);
    if (!showID.has_value())
    {
        LOG_ERROR<<std::format("showID пустой для {}",showTitle);
        return {};
    }

    id = createTVSeasonMediaItem(scanSettings, *showID, season, releaseDate);
    if (!id.has_value())
    {
        LOG_DEBUG<<std::format("Не смогли создать новый сезон для созданого сериала");
        return {};
    }
    return id;
}

void parseFileName(const std::string& fileName, std::string& name, int& season, int& episode, int& year)
{
    LOG_DEBUG<<std::format("passed name {}", fileName);
    int dotCount{},spaceCount{};
    bool isMovie = true;
    std::vector<std::string> splitString;
    std::string sep;
    std::string str = std::move(cleanupFileNameCopy(fileName));
    std::for_each(str.begin(), str.end(), [&dotCount, &spaceCount](char ch){
        if (ch == '.')
        {
            dotCount++;
            return;
        }
        if (ch == ' ')
            spaceCount++;
    });
    if (dotCount > spaceCount)
    {
        sep = ".";
        splitString.reserve(dotCount+1);
    }
    else 
    {
        sep = " ";
        splitString.reserve(spaceCount+1);
    }
    std::smatch match;
    //std::regex_search(fileName, match, isSeasonEpisodePattern);
    if (std::regex_search(fileName, match, isSeasonEpisodePattern))
    {
        std::string s = match[0].str();
        boost::trim(s);
        getSeasonEpisode(s, season, episode);
        str = std::move(str.substr(0, str.find(s) - 1));
        isMovie = false;
    }

    boost::split(splitString, str, boost::is_any_of(sep));
    if (splitString.empty())
        return;

    std::vector<std::string>::size_type end = 0;
    for (auto it = splitString.rbegin(); it != splitString.rend(); it++)
    {
        auto itt = it;
        ++itt;
        if (itt == splitString.rend())
            break;
        if (isNumber(*it) && (isMovie || isNumber(*itt)))
        {
            year = strToInt(*it);
            if (year > 1000)
            {
                end = std::distance(splitString.begin(), it.base()) - 1;
                break;
            }
            else
                year = 0;
        }
    }
    name = splitStrToStr(splitString, " ", 0, end);
    boost::trim(name);
    auto len = name.size();
    // LOG_DEBUG<<std::format("name {}, last char {}", name, name.at(len-1));
    if (isElementInContainer(name.at(len-1), badSymbols))
    {
        name = name.substr(0, len-2);
        // LOG_DEBUG<<std::format("name {}", name);
        boost::trim(name);
    }
    LOG_DEBUG<<std::format("parseFileName_Result {}, {}, {}, {}", name, season, episode, year);
}


std::optional<models::MediaItems> createMediaItem(const LibraryScanSettings& scanSettings, MediaType type, int64_t parentID, const std::string& parsedTitle, const std::string& path, const int season, const int episode, const std::string& releaseDate, orm::DbClientPtr dbPointer)
{
    // std::lock_guard<std::mutex> lockMutex(createSeasonMutex);
    LOG_DEBUG<<std::format("CreateMediaItem({}, {}, {}, {}, {}, {})", enumToInt(type), parentID, parsedTitle, season, episode, releaseDate);
    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();
   
    models::MediaItems ret;

    ret.setMediaItemTypeId(enumToInt(type));
    if (type == MediaType::TVEpisode || type == MediaType::TVSeason)
    {
        LOG_ERROR<<std::format("parent_id {}", parentID);
        ret.setParentId(parentID);
    }
    else
        ret.setParentIdToNull();
    // if (season <= 0)
    // {
    //     ret.setSeasonToNull();
    //     ret.setEpisodeToNull();
    // }
    // else 
    // {
    //     ret.setSeason(season);
    //     ret.setEpisode(episode);
    // }
    if (season <= 0)
        ret.setSeasonToNull();
    else
        ret.setSeason(season);

    if (episode <= 0 && type != MediaType::TVSeason)
        ret.setEpisodeToNull();
    else
        ret.setEpisode(episode);
    
    // ret.setEpisode(episode);
    //ret.setPath("");
    if (path.empty())
        ret.setPathToNull();
    else
        ret.setPath(path);
    if (parsedTitle.empty())
        ret.setParsedNameToNull();
    else
        ret.setParsedName(parsedTitle);
    // if (year == 0)
    //     ret.setReleaseYearToNull();
    // else
    //     ret.setReleaseYear(year);
    if (releaseDate.empty())
        ret.setReleaseDateToNull();
    else
        ret.setReleaseDate(releaseDate);

    ret.setLastUpdated(getCurrentDateTime());


    // if (type == MediaType::TVShow)
    // {
    //     // orm::Criteria(models::MediaItems::Cols::_release_year, orm::CompareOperator::EQ, year) && 
    //     auto find = findRecordByCriteriaORM<models::MediaItems>(orm::Criteria(models::MediaItems::Cols::_parsed_name, orm::CompareOperator::EQ, parsedTitle)
    //     && orm::Criteria(models::MediaItems::Cols::_media_item_type_id, orm::CompareOperator::EQ, enumToInt(type)), dbPointer);
    //     if (find.has_value())
    //     {
    //         LOG_DEBUG<<std::format("Уже есть такое шоу {}", find->getValueOfParsedName());
    //         return find;
    //     }
    // }
    // findShowMediaItemID
    orm::Mapper<models::MediaItems> mp(dbPointer);
    try
    {   
        mp.insert(ret);
        return ret;
    }
    catch (const drogon::orm::UniqueViolation& e)
    {
        LOG_ERROR<<std::format("Ошибка запроса unique: {} CreateMediaItem({}, {}, {}, {}, {}, {})", e.what() ,enumToInt(type), parentID, parsedTitle, season, episode, releaseDate);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<std::format("Ошибка запроса: {} CreateMediaItem({}, {}, {}, {}, {}, {})", e.base().what(),enumToInt(type), parentID, parsedTitle, season, episode, releaseDate);
        return {};
    }
    return {};
}

std::optional<int64_t> createTVShowMediaItem(const LibraryScanSettings& scanSettings, const std::string& parsedTitle, const std::string& releaseDate, orm::DbClientPtr dbPointer)
{
    LOG_DEBUG<<std::format("createTVShowMediaItem: {}, {}", parsedTitle, releaseDate);
    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();

    std::optional<models::MediaItems> tvShow = createMediaItem(scanSettings, MediaType::TVShow, 0, parsedTitle, "", 0, 0, releaseDate);
    if (!tvShow.has_value())
        return {};
    getTVShowMetaData(scanSettings, *tvShow);
    return tvShow->getPrimaryKey();
}

std::optional<int64_t> createTVSeasonMediaItem(const LibraryScanSettings& scanSettings, const int64_t parentID, const int season,  const std::string& releaseDate, orm::DbClientPtr dbPointer)
{
    if (!dbPointer)
       dbPointer = drogon::app().getDbClient();

    std::optional<models::MediaItems> tvSeason = createMediaItem(scanSettings, MediaType::TVSeason, parentID, "", "", season, 0, releaseDate);
    if (!tvSeason.has_value())
        return {};
    getTVSeasonMetaData(scanSettings, *tvSeason, getMediaItemExternalID(parentID));
    return tvSeason->getPrimaryKey();
}

std::vector<models::ExternalMediaItemIds> getMediaItemExternalID(const int64_t mediaItemID, orm::DbClientPtr dbPointer)
{
    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();
    std::vector<models::ExternalMediaItemIds> ret{};
    orm::Mapper<models::MediaItems> mp(dbPointer);
    try
    {
        ret = mp.findByPrimaryKey(mediaItemID).getExternalMediaItemIds(dbPointer);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
    }
    return ret;
}

std::vector<models::ExternalMediaItemIds> getShowExternalIDFromSeason(const int64_t seasonMediaItemID, orm::DbClientPtr dbPointer)
{
    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();
    std::vector<models::ExternalMediaItemIds> ret{};
    orm::Mapper<models::MediaItems> mp(dbPointer);
    try
    {
        auto season = mp.findByPrimaryKey(seasonMediaItemID);
        ret = mp.findByPrimaryKey(season.getValueOfParentId()).getExternalMediaItemIds(dbPointer);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
    }
    return ret;
}

std::vector<models::ExternalMediaItemIds> getMediaItemExternalID(const models::MediaItems& mediaItem, orm::DbClientPtr dbPointer)
{
    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();
    return mediaItem.getExternalMediaItemIds(dbPointer);
}

bool extIDInVec(const std::vector<models::ExternalMediaItemIds>& showExternalID, const MetaDataProvider provider, std::string& extID)
{
    auto it = std::find_if(showExternalID.begin(), showExternalID.end(), [provider](const models::ExternalMediaItemIds& extID){
        if (static_cast<MetaDataProvider>(extID.getValueOfMetadataProviderId()) == provider)
            return true;
        return false;
    });
    if(it != showExternalID.end())
    {
        extID = it->getValueOfExternalId();
        LOG_INFO<<std::format("extID {}", extID);
        return true;
    }
    else
    {
        extID.clear();
        return false;
    }
}

bool getTVSeasonMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem, const int showID)
{
    bool parsed = false;
    int seasonNum = mediaItem.getValueOfSeason();
    auto season = TMDBAPI::Endpoints::TVSeason(showID, mediaItem.getValueOfSeason());
    auto seasonDetails = season.getDetails();
    if (!seasonDetails.has_value())
    {
        LOG_ERROR<<std::format("seasonDetails.Error {}", enumToInt(seasonDetails.error()));
        return parsed;
    }
    mediaItem.setReleaseDate((*seasonDetails).details.airDate);
    int64_t mediaItemID = mediaItem.getPrimaryKey();
    std::vector<std::future<bool>> futures;
    futures.reserve(4);
    auto tvSeasonExtIDs = TMDBAPI::Endpoints::TVSeries(showID).getExternalIDs();
    if (tvSeasonExtIDs.has_value())
    {
        futures.emplace_back(metaDataThreadPool.submit_task([&tvSeasonExtIDs, mediaItemID]{
            updateExtIds(*tvSeasonExtIDs, mediaItemID);
            return false;}));
    }
    Language originalLanguage = getOriginalLanguage(mediaItem.getValueOfParentId());
    // if (originalLanguage != Language::xx && !isElementInContainer(originalLanguage, scanSettings.languagesToScanFor))
    // {
    //     futures.emplace_back(metaDataThreadPool.submit_task([&scanSettings, showID, seasonNum, mediaItemID, originalLanguage]{return getTVSeasonMetaDataTMDB(scanSettings, showID, seasonNum, mediaItemID, originalLanguage, true);}));
    //     // futures.emplace_back(threadPool.submit_task([&]{getTVSeasonMetaDataTMDB(scanSettings, showID, mediaItem.getValueOfSeason(), mediaItemID, originalLanguage, true);}));
    // }
    for (const Language language : scanSettings.languagesToScanFor)
    {
        futures.emplace_back(metaDataThreadPool.submit_task([&scanSettings, showID, seasonNum, mediaItemID, language, originalLanguage]{return getTVSeasonMetaDataTMDB(scanSettings, showID, seasonNum, mediaItemID, language, language == originalLanguage);}));

    }
    
    updateRecord(mediaItem);
    for (auto& future : futures)
        parsed |= future.get();
    return parsed;
}

bool getTVEpisodeMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem, const int TMDBShowID)
{
    bool parsed = false;
    int season, episode;
    season = mediaItem.getValueOfSeason();
    episode = mediaItem.getValueOfEpisode();
    auto episodeDetails = TMDBAPI::Endpoints::TVEpisode(TMDBShowID, season, episode).getDetails();
    if (!episodeDetails.has_value())
    {
        if (episodeDetails.error() == TMDBAPI::APIError::ResourceNotFound)
        {
            LOG_INFO<<std::format("Не удалось получить эпизод");
            // std::lock_guard lock();
            
            auto tvSeasonDetails = TMDBAPI::Endpoints::TVSeries(TMDBShowID).getDetails();
            if (!tvSeasonDetails.has_value())
                return false;
            auto seasonCount = (*tvSeasonDetails).details.seasons.size();
            // Значит промахнулись в меньшую сторону
            int newSeason = season + 1;
            int newEpisode = episode;
            if (season < seasonCount && newSeason <= seasonCount)
            {
                int episodeCount = 0;
                for (int i = 0; i < newSeason; i++)
                {
                    episodeCount += (*tvSeasonDetails).details.seasons[i].episodeCount;
                }
                if (episodeCount < episode)
                    return false;
                newEpisode = episodeCount - episode + 1;
                mediaItem.setSeason(newSeason);
                mediaItem.setEpisode(newEpisode);
                std::string newName = mediaItem.getValueOfParsedName();
                newName = newName.substr(0, newName.rfind('S') - 1);
                LOG_ERROR<<std::format("new parsed name {}", newName);
                auto newParent = getParentForEpisode(newName, newSeason, mediaItem.getValueOfReleaseDate(), scanSettings);
                newName += std::format(" S{}E{}", newSeason, newEpisode);
                LOG_ERROR<<std::format("new name {}", newName);
                if (!newParent.has_value())
                    return false;
                mediaItem.setParsedName(newName);
                mediaItem.setParentId(*newParent);
                if (!updateRecord(mediaItem))
                    return false;
            }
            season = newSeason;
            episode = newEpisode;
        }
        else
        {
            LOG_ERROR<<std::format("episodeDetails.Error {}", enumToInt(episodeDetails.error()));
            return parsed;
        }
    }
    int64_t mediaItemID = mediaItem.getPrimaryKey();
    std::vector<std::future<bool>> futures;
    futures.reserve(6);
    auto tvEpisodeExtIDs = TMDBAPI::Endpoints::TVEpisode(TMDBShowID, season, episode).getExternalIDs();
    if (tvEpisodeExtIDs.has_value())
    {
        futures.emplace_back(metaDataThreadPool.submit_task([&tvEpisodeExtIDs, mediaItemID]{
            updateExtIds(*tvEpisodeExtIDs, mediaItemID);
            return false;}));
    }
    Language originalLanguage = getOriginalLanguage(mediaItem.getValueOfParentId());
    // if (originalLanguage != Language::xx && !isElementInContainer(originalLanguage, scanSettings.languagesToScanFor))
    // {
    //     futures.emplace_back(metaDataThreadPool.submit_task([&scanSettings, TMDBShowID, season, episode, mediaItem, originalLanguage, mediaItemID]{return getTVEpisodeMetaDataTMDB(scanSettings, TMDBShowID, season, episode, mediaItemID, originalLanguage, true);}));
    //     // futures.emplace_back(threadPool.submit_task([&]{getTVSeasonMetaDataTMDB(scanSettings, showID, mediaItem.getValueOfSeason(), mediaItemID, originalLanguage, true);}));
    // }
    for (const Language language : scanSettings.languagesToScanFor)
    {
        // LOG_ERROR<<"AAAAAAAAAAAAA "<<enumToInt(language);
        futures.emplace_back(metaDataThreadPool.submit_task([&scanSettings, TMDBShowID, season, episode, mediaItem, language, originalLanguage, mediaItemID]{return getTVEpisodeMetaDataTMDB(scanSettings, TMDBShowID, season, episode, mediaItemID, language, language == originalLanguage);}));
    }
    for (auto& future : futures)
        parsed |= future.get();
    return parsed;
}

bool getTVEpisodeMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBShowID, const int season, const int episode, const int64_t mediaItemID, const Language language, const bool original)
{
    std::vector<std::future<void>> futures;
    futures.reserve(3);

    LOG_DEBUG<<std::format("getTVEpisodeMetaDataTMDB({}, {}, {}, {}, {})", TMDBShowID, season, episode, mediaItemID, enumToInt(language));

    auto tvEpisodeDetails = TMDBAPI::Endpoints::TVEpisode(TMDBShowID, season, episode).getDetails(languageToTMDBLanguage(language),
     TMDBAPI::TVAppendToResponse::Images | TMDBAPI::TVAppendToResponse::Credits, {languageToTMDBLanguage(language), TMDBAPI::Languages::Xx});
    // auto tvEpisodeDetails = TMDBAPI::Endpoints::TVEpisode(TMDBShowID, season, episode).getDetails(languageToTMDBLanguage(language), TMDBAPI::TVAppendToResponse::Images);
    if (!tvEpisodeDetails.has_value())
    {
        LOG_INFO<<std::format("Не удалось получить ТВ эпизода S{}E{} шоу с ID {} , ошибка {}", season, episode, TMDBShowID, enumToInt(tvEpisodeDetails.error()));
        return false;
    }
    futures.emplace_back(metaDataThreadPool.submit_task([&tvEpisodeDetails, mediaItemID, language, original]{
        insertMediaItemLocalization(mediaItemID, language, tvEpisodeDetails->details.name, tvEpisodeDetails->details.overview, "", original);
    }));

    if (tvEpisodeDetails->credits.has_value() && scanSettings.collectEpisodeCredits)
    {
        if (!tvEpisodeDetails->credits->cast.empty())
            futures.emplace_back(metaDataThreadPool.submit_task([&tvEpisodeDetails, mediaItemID, language]{
                insertCredits(tvEpisodeDetails->credits->cast, language, mediaItemID);
            }));
        if (!tvEpisodeDetails->credits->crew.empty())
            futures.emplace_back(metaDataThreadPool.submit_task([&tvEpisodeDetails, mediaItemID, language]{
                insertCredits(tvEpisodeDetails->credits->crew, language, mediaItemID);
            }));
    }
    //  if (!tvEpisodeDetails->details.crew.empty())
    //         futures.emplace_back(metaDataThreadPool.submit_task([&tvEpisodeDetails, mediaItemID, language]{
    //             insertCredits(tvEpisodeDetails->credits->cast, language, mediaItemID);
    //         }));
    //     if (!tvEpisodeDetails->credits->crew.empty())
    //         futures.emplace_back(metaDataThreadPool.submit_task([&tvEpisodeDetails, mediaItemID, language]{
    //             insertCredits(tvEpisodeDetails->credits->crew, language, mediaItemID);
    //         }));
    // Подумать над локальными изображениями
    // if (!(*tvEpisodeDetails).details.stillPath.empty())
    // {
        
    //     futures.emplace_back(metaDataThreadPool.submit_task([language, &tvEpisodeDetails, &scanSettings, mediaItemID]{
            
    //         insertImage(scanSettings, ImageType::Still, language, "https://image.tmdb.org/t/p/original"+(*tvEpisodeDetails).details.stillPath, mediaItemID);
    //     }));
    // }
    if ((*tvEpisodeDetails).images.has_value())
    {
        if ((*(*tvEpisodeDetails).images).stills.size() > 0 && !hasImage(mediaItemID, ImageType::Still, language))
            futures.emplace_back(metaDataThreadPool.submit_task([language, &tvEpisodeDetails, &scanSettings, mediaItemID]{
                LOG_INFO<<(*(*tvEpisodeDetails).images).stills[0].filePath;
                insertImage(scanSettings, ImageType::Still, language, (*(*tvEpisodeDetails).images).stills[0].getImageLink(TMDBAPI::StillSize::Original), mediaItemID);
            }));
    }

    for (auto& future : futures)
        future.wait();
    return true;
}

bool getTVShowMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBid, const int64_t mediaItemID, const Language language, const bool original)
{
    LOG_DEBUG<<std::format("getTVShowMetaDataTMDB({}, {}, {})", TMDBid, mediaItemID, enumToInt(language));

    std::vector<std::future<void>> futures;
    auto tvShowDetails = TMDBAPI::Endpoints::TVSeries(TMDBid).getDetails(languageToTMDBLanguage(language),
     TMDBAPI::TVAppendToResponse::Images | TMDBAPI::TVAppendToResponse::Credits, {languageToTMDBLanguage(language), TMDBAPI::Languages::Xx});
    if (!tvShowDetails.has_value())
    {
        LOG_INFO<<std::format("Не удалось получить ТВ шоу с ID {}, ошибка {}", TMDBid, enumToInt(tvShowDetails.error()));
        return false;
    }
    
    futures.emplace_back(metaDataThreadPool.submit_task([&tvShowDetails, mediaItemID, language, original]{
        insertMediaItemLocalization(mediaItemID, language, tvShowDetails->details.name, tvShowDetails->details.overview, tvShowDetails->details.tagline, original);
    }));

    if (tvShowDetails->credits.has_value() && scanSettings.collectShowCredits)
    {
        LOG_DEBUG<<std::format("aggregateCredits size: cast {}, crew {}", tvShowDetails->credits->cast.size(), tvShowDetails->credits->crew.size());
        if (!tvShowDetails->credits->cast.empty())
            futures.emplace_back(metaDataThreadPool.submit_task([&tvShowDetails, mediaItemID, language]{
                insertCredits(tvShowDetails->credits->cast, language, mediaItemID);
            }));
        // futures.emplace_back(metaDataThreadPool.submit_task([language, &tvShowDetails, &scanSettings, mediaItemID]{
            
        //     insertImage(scanSettings, ImageType::Poster, language, "https://image.tmdb.org/t/p/original"+(*tvShowDetails).details.posterPath, mediaItemID);
        // }));
        // if (!tvShowDetails->credits->crew.empty())
        //     futures.emplace_back(metaDataThreadPool.submit_task([&tvShowDetails, mediaItemID, language]{
        //         insertCredits(tvShowDetails->credits->crew, language, mediaItemID);
        //     }));
    }
    // Подумать над локальными изображениями
    if ((*tvShowDetails).images.has_value())
    {

        if ((*(*tvShowDetails).images).backdrops.size() > 0  && !hasImage(mediaItemID, ImageType::Background, language))
            futures.emplace_back(metaDataThreadPool.submit_task([language, &tvShowDetails, &scanSettings, mediaItemID]{
                LOG_INFO<<(*(*tvShowDetails).images).backdrops[0].filePath;

                insertImage(scanSettings, ImageType::Background, language, (*(*tvShowDetails).images).backdrops[0].getImageLink(TMDBAPI::BackdropSize::Original), mediaItemID);
            }));
        if ((*(*tvShowDetails).images).posters.size() > 0  && !hasImage(mediaItemID, ImageType::Poster, language))
            futures.emplace_back(metaDataThreadPool.submit_task([language, &tvShowDetails, &scanSettings, mediaItemID]{
                LOG_INFO<<(*(*tvShowDetails).images).posters[0].filePath;

                insertImage(scanSettings, ImageType::Poster, language, (*(*tvShowDetails).images).posters[0].getImageLink(TMDBAPI::PosterSize::Original), mediaItemID);
            }));
            
    }

    if (!tvShowDetails->details.genres.empty())
    {
        futures.emplace_back(metaDataThreadPool.submit_task([mediaItemID, &tvShowDetails, language]{
            for (const auto& genre : tvShowDetails->details.genres)
            {
                insertGenre(mediaItemID, MetaDataProvider::TMDB, std::to_string(genre.id), genre.name, language);
            }
        }));
    }

    if (!tvShowDetails->details.productionCompanies.empty())
    {
        futures.emplace_back(metaDataThreadPool.submit_task([mediaItemID, &tvShowDetails]{
            for (const auto& company : tvShowDetails->details.productionCompanies)
            {
                insertProductionCompany(mediaItemID, company.name);
            }
        })
        );
    }
    for (auto& future : futures)
        future.wait();
    return true;
}

bool getTVSeasonMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBShowID, const int TMDBSeasonID, const int64_t mediaItemID, const Language language, const bool original)
{
    LOG_DEBUG<<std::format("getTVSeasonMetaDataTMDB({}, {}, {}, {})", TMDBShowID, TMDBSeasonID, mediaItemID, enumToInt(language));
    std::vector<std::future<void>> futures;
    futures.reserve(5);
    auto seasonDetails = TMDBAPI::Endpoints::TVSeason(TMDBShowID, TMDBSeasonID).getDetails(languageToTMDBLanguage(language), TMDBAPI::TVSeasonAppendToResponse::Credits);
    // auto seasonDetails = TMDBAPI::Endpoints::TVSeason(TMDBShowID, TMDBSeasonID).getDetails(languageToTMDBLanguage(language));
    if (!seasonDetails.has_value())
    {
        //error
        LOG_ERROR<<std::format("seasonDetails.Error {}", enumToInt(seasonDetails.error()));
        return false;
    }
    futures.emplace_back(metaDataThreadPool.submit_task([&seasonDetails, mediaItemID, language, original]{
        insertMediaItemLocalization(mediaItemID, language, seasonDetails->details.name, seasonDetails->details.overview, "", original);
    }));
    if (seasonDetails->credits.has_value() && scanSettings.collectSeasonCredits)
    {
        futures.emplace_back(metaDataThreadPool.submit_task([&seasonDetails, language, mediaItemID]{
            insertCredits(seasonDetails->credits->crew, language, mediaItemID);
        }));
        futures.emplace_back(metaDataThreadPool.submit_task([&seasonDetails, language, mediaItemID]{
            insertCredits(seasonDetails->credits->cast, language, mediaItemID);
        }));
        // futures.emplace_back(metaDataThreadPool.submit_task([&seasonDetails, language, mediaItemID]{
        //     insertCredits(seasonDetails->credits->guestStars, language, mediaItemID);
        // }));
    }
    // Подумать над локальными изображениями
    if ((*seasonDetails).images.has_value())
    {
        if ((*(*seasonDetails).images).posters.size() > 0 && !hasImage(mediaItemID, ImageType::Poster, language))
            futures.emplace_back(metaDataThreadPool.submit_task([language, &seasonDetails, &scanSettings, mediaItemID]{
                insertImage(scanSettings, ImageType::Poster, language, (*(*seasonDetails).images).posters[0].getImageLink(TMDBAPI::PosterSize::Original), mediaItemID);
            }));
    }
    for (auto& future : futures)
        future.wait();
    return true;
}

bool getMovieMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBid, const int64_t mediaItemID, const Language language, const bool original)
{
    LOG_DEBUG<<std::format("getMovieMetaDataTMDB({}, {}, {})", TMDBid, mediaItemID, enumToInt(language));
    std::vector<std::future<void>> futures;
    futures.reserve(5);
    // auto movieDetails = TMDBAPI::Endpoints::Movie(TMDBid).getDetails(languageToTMDBLanguage(language), TMDBAPI::MovieAppendToResponse::Images | TMDBAPI::MovieAppendToResponse::Credits);
    auto movieDetails = TMDBAPI::Endpoints::Movie(TMDBid).getDetails(languageToTMDBLanguage(language),
     TMDBAPI::MovieAppendToResponse::Images | TMDBAPI::MovieAppendToResponse::Credits, {languageToTMDBLanguage(language), TMDBAPI::Languages::Xx});
    if (!movieDetails.has_value())
    {
        LOG_INFO<<std::format("Не удалось получить Фильм с ID {}, ошибка {}", TMDBid, enumToInt(movieDetails.error()));
        return false;
    }
    futures.reserve(4);
    futures.emplace_back(metaDataThreadPool.submit_task([&movieDetails, mediaItemID, language, original]{
        insertMediaItemLocalization(mediaItemID, language, (*movieDetails).details.title, movieDetails->details.overview, movieDetails->details.tagline, original);
    }));

    if ((*movieDetails).credits.has_value() && scanSettings.collectMovieCredits)
    {
        futures.emplace_back(metaDataThreadPool.submit_task([&movieDetails, language, mediaItemID]{
            insertCredits((*movieDetails).credits->crew, language, mediaItemID);
        }));
        futures.emplace_back(metaDataThreadPool.submit_task([&movieDetails, language, mediaItemID]{
            insertCredits((*movieDetails).credits->cast, language, mediaItemID);
        }));
        // futures.emplace_back(metaDataThreadPool.submit_task([&seasonDetails, language, mediaItemID]{
        //     insertCredits(seasonDetails->credits->guestStars, language, mediaItemID);
        // }));`
    }
    if ((*movieDetails).images.has_value())
    {
        if ((*(*movieDetails).images).posters.size() > 0 && !hasImage(mediaItemID, ImageType::Poster, language))
            futures.emplace_back(metaDataThreadPool.submit_task([language, &movieDetails, &scanSettings, mediaItemID]{
                insertImage(scanSettings, ImageType::Poster, language, (*(*movieDetails).images).posters[0].getImageLink(TMDBAPI::PosterSize::Original), mediaItemID);
            }));
        if ((*(*movieDetails).images).logos.size() > 0 && !hasImage(mediaItemID, ImageType::Logo, language))
            futures.emplace_back(metaDataThreadPool.submit_task([language, &movieDetails, &scanSettings, mediaItemID]{
                insertImage(scanSettings, ImageType::Logo, language, (*(*movieDetails).images).logos[0].getImageLink(TMDBAPI::PosterSize::Original), mediaItemID);
            }));
        if ((*(*movieDetails).images).backdrops.size() > 0 && !hasImage(mediaItemID, ImageType::Background, language))
            futures.emplace_back(metaDataThreadPool.submit_task([language, &movieDetails, &scanSettings, mediaItemID]{
                insertImage(scanSettings, ImageType::Background, language, (*(*movieDetails).images).backdrops[0].getImageLink(TMDBAPI::PosterSize::Original), mediaItemID);
            }));
    }
    for (auto& future : futures)
        future.wait();
    return true;
}

bool getMovieMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem)
{
    bool parsed = false;
    auto search = TMDBAPI::Endpoints::SearchMovie(mediaItem.getValueOfParsedName(), scanSettings.includeAdult);
    int year = getYearFromDate(mediaItem.getValueOfReleaseDate());
    if (year != 0)
        search.setYear(year);
    auto searchResults = search.getSearchResult();
    if (!searchResults.has_value())
    {
        LOG_INFO<<std::format("Запрос {} вернул код ошибки {}", search.getQuery(), enumToInt(searchResults.error()));
        return parsed;
    }
    if (searchResults->empty())
    {
        LOG_INFO<<std::format("по запросу {} не найдено результатов", search.getQuery());
        return parsed;
    }
    // всегда берем первый результат
    auto searchResult = (*searchResults.value().begin()).results[0];
    mediaItem.setReleaseDate(searchResult.releaseDate);
    int TMDBid = searchResult.id;
    auto mediaItemID = mediaItem.getPrimaryKey();
    std::vector<std::future<bool>> futures;
    futures.reserve(scanSettings.languagesToScanFor.size() + 2);
    // Безопасно потому что передаем аргументы по копии
    auto movieExtIDs = TMDBAPI::Endpoints::Movie(TMDBid).getExternalIDs();
    if (movieExtIDs.has_value())
    {
        futures.emplace_back(metaDataThreadPool.submit_task([&movieExtIDs, mediaItemID]{
            updateExtIds(*movieExtIDs, mediaItemID);
            return false;}));
    }
    Language originalLanguage = TMDBLanguageToLanguage(TMDBAPI::strToLanguage(searchResult.originalLanguage));
    LOG_DEBUG<<std::format("Original language {}", enumToInt(originalLanguage));
    // mediaItem.setReleaseYear(searchResult.);
    // if (!isElementInContainer(originalLanguage, scanSettings.languagesToScanFor))
    //     futures.emplace_back(metaDataThreadPool.submit_task([&scanSettings, TMDBid, mediaItemID, originalLanguage]{
    //         return getMovieMetaDataTMDB(scanSettings, TMDBid, mediaItemID, originalLanguage, true);
    //     }));
    for (const Language language : scanSettings.languagesToScanFor)
    {
        futures.emplace_back(metaDataThreadPool.submit_task([&scanSettings, TMDBid, mediaItemID, language, originalLanguage]{
           return getMovieMetaDataTMDB(scanSettings, TMDBid, mediaItemID, language, language == originalLanguage);
        }));
    }
    for (auto& future : futures)
        parsed |= future.get();
    updateRecord(mediaItem);
    return parsed;
}



void updateExtIds(const TMDBAPI::Models::PersonExternalIds& extIDs, const int64_t personID)
{
    // futures.emplace_back(threadPool.submit_task([&extIDs, personID]{insertPersonExternalID(personID, MetaDataProvider::TMDB, std::to_string(extIDs.id));}));
    insertPersonExternalID(personID, MetaDataProvider::TMDB, std::to_string(extIDs.id));
    // if (!extIDs.imdbId.empty())
    //     futures.emplace_back(threadPool.submit_task([&extIDs, personID]{insertPersonExternalID(personID, MetaDataProvider::IMDB, extIDs.imdbId);}));
    // for (auto& future : futures)
    //     future.wait();
}

void updateExtIds(const TMDBAPI::Models::MovieExternalIds& extIDs, const int64_t mediaItemID)
{
    // futures.emplace_back(threadPool.submit_task([&]{insertMediaItemExternalID(mediaItemID, MetaDataProvider::TMDB, std::to_string(extIDs.id));}));
    // if (!extIDs.imdbId.empty())
    //     futures.emplace_back(threadPool.submit_task([&extIDs, mediaItemID]{insertMediaItemExternalID(mediaItemID, MetaDataProvider::IMDB, extIDs.imdbId);}));
    insertMediaItemExternalID(mediaItemID, MetaDataProvider::TMDB, std::to_string(extIDs.id));
}

void updateExtIds(const TMDBAPI::Models::TVSeasonExternalIds& extIDs, const int64_t mediaItemID)
{
    // futures.emplace_back(threadPool.submit_task([&]{insertMediaItemExternalID(mediaItemID, MetaDataProvider::TMDB, std::to_string(extIDs.id));}));
    insertMediaItemExternalID(mediaItemID, MetaDataProvider::TMDB, std::to_string(extIDs.id));
}

void updateExtIds(const TMDBAPI::Models::TVSeriesExternalIds& extIDs, const int64_t mediaItemID)
{
    insertMediaItemExternalID(mediaItemID, MetaDataProvider::TMDB, std::to_string(extIDs.id));
//     futures.emplace_back(threadPool.submit_task([&]{insertMediaItemExternalID(mediaItemID, MetaDataProvider::TMDB, std::to_string(extIDs.id));}));
//     if (!extIDs.imdbId.empty())
//         futures.emplace_back(threadPool.submit_task([&extIDs, mediaItemID]{insertMediaItemExternalID(mediaItemID, MetaDataProvider::IMDB, extIDs.imdbId);}));
//     for (auto& future : futures)
//         future.wait();
}

void updateExtIds(const TMDBAPI::Models::TVEpisodeExternalIds& extIDs, const int64_t mediaItemID)
{
    insertMediaItemExternalID(mediaItemID, MetaDataProvider::TMDB, std::to_string(extIDs.id));
    if (!extIDs.imdbId.empty())
        insertMediaItemExternalID(mediaItemID, MetaDataProvider::IMDB, extIDs.imdbId);
}

bool getTVShowMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem)
{
    bool parsed = false;

    auto search = TMDBAPI::Endpoints::SearchTV(mediaItem.getValueOfParsedName(), scanSettings.includeAdult);
    int year = getYearFromDate(mediaItem.getValueOfReleaseDate());
    if (year != 0)
        search.setYear(year);
    auto searchResults = search.getSearchResult();
    if (!searchResults.has_value())
    {
        LOG_INFO<<std::format("Запрос {} вернул код ошибки {}", search.getQuery(), enumToInt(searchResults.error()));
        return parsed;
    }
    if (searchResults->empty())
    {
        LOG_INFO<<std::format("по запросу {} не найдено результатов", search.getQuery());
        return parsed;
    }
    // всегда берем первый результат
    auto searchResult = (*searchResults.value().begin()).results[0];
    int TMDBid = searchResult.id;
    mediaItem.setReleaseDate(searchResult.releaseDate);
    auto mediaItemID = mediaItem.getPrimaryKey();
        
    std::vector<std::future<bool>> futures;
    futures.reserve(scanSettings.languagesToScanFor.size() + 2);
    auto tvShowExtIDs = TMDBAPI::Endpoints::TVSeries(TMDBid).getExternalIDs();
    if (tvShowExtIDs.has_value())
    {
        futures.emplace_back(metaDataThreadPool.submit_task([&tvShowExtIDs, mediaItemID]{
            updateExtIds(*tvShowExtIDs, mediaItemID);
            return false;}));
    }
    Language originalLanguage = TMDBLanguageToLanguage(TMDBAPI::strToLanguage(searchResult.originalLanguage));
    // LOG_DEBUG<<std::format("Original Language {}", enumToInt(originalLanguage));
    // mediaItem.setReleaseYear(searchResult.);
    // if (!isElementInContainer(originalLanguage, scanSettings.languagesToScanFor))
    //     futures.emplace_back(metaDataThreadPool.submit_task([&scanSettings, TMDBid, mediaItemID, originalLanguage]{
    //         return getTVShowMetaDataTMDB(scanSettings, TMDBid, mediaItemID, originalLanguage, true);
    //     }));
    for (const Language language : scanSettings.languagesToScanFor)
    {
        futures.emplace_back(metaDataThreadPool.submit_task([&scanSettings, TMDBid, mediaItemID, language, originalLanguage]{
           return getTVShowMetaDataTMDB(scanSettings, TMDBid, mediaItemID, language, language == originalLanguage);
        }));
    }
    updateRecord(mediaItem);
    for (auto& future : futures)
        parsed |= future.get();
    return parsed;
}



bool getTVSeasonMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem, const std::vector<models::ExternalMediaItemIds>& showExternalIDs)
{
    if (scanSettings.metaDataProvider == MetaDataProvider::Local)
        return true;

    bool parsed = false;
    std::string extID{};
    // вынести в отдельную функцию
    if(hasFlag(scanSettings.metaDataProvider, MetaDataProvider::TMDB) && extIDInVec(showExternalIDs, MetaDataProvider::TMDB, extID))
    {
        LOG_ERROR<<std::format("Ищем сезон в TMDBAPI {}", extID);
        parsed = getTVSeasonMetaDataTMDB(scanSettings, mediaItem, std::stoi(extID));
    }
    return parsed;
}

std::vector<int64_t> getAssignedGenres(const int64_t mediaItemID)
{
    auto assignments = getGenreAssignments(mediaItemID);
    if (assignments.empty())
        return {};
    std::vector<int64_t> ret;
    ret.reserve(assignments.size());
    for (const auto& assignment : assignments)
        ret.push_back(assignment.getValueOfGenreId());
    return ret;
}

void copyGenres(const int64_t fromMediaItemID, const int64_t toMediaItemID)
{
    auto genres = getGenreAssignments(fromMediaItemID);
    orm::Mapper<models::GenreAssignments> mp(app().getDbClient());
    orm::Mapper<models::GenreAssignments> mp1(app().getDbClient());
    models::GenreAssignments assignment;
    for (auto& genre : genres)
    {
        if(isGenreAssigned(genre.getValueOfGenreId(), toMediaItemID))
            continue;
        genre.setMediaItemId(toMediaItemID);
        insertRecord(genre, mp);
    }

}


bool getTVShowMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem)
{
    if (scanSettings.metaDataProvider == MetaDataProvider::Local)
        return true;
    bool parsed = false;

    // вынести в отдельную функцию
    if(hasFlag(scanSettings.metaDataProvider, MetaDataProvider::TMDB))
    {
        LOG_ERROR<<std::format("Ищем шоу в TMDBAPI {}", mediaItem.getValueOfParsedName());
        parsed = getTVShowMetaDataTMDB(scanSettings, mediaItem);
    }

    if(hasFlag(scanSettings.metaDataProvider, MetaDataProvider::KinoPoisk))
    {

    }
    return parsed;
}

std::optional<int64_t> findGenre(const MetaDataProvider provider, const std::string& extID, orm::DbClientPtr dbPointer)
{
    return findRecordByCriteria<models::Genres>(orm::Criteria(models::Genres::Cols::_origin, orm::CompareOperator::EQ, enumToInt(provider))
                                                && orm::Criteria(models::Genres::Cols::_external_id, orm::CompareOperator::EQ, extID));
}

std::optional<int64_t> insertGenre(const MetaDataProvider provider, const std::string& extID, orm::DbClientPtr dbPointer)
{
    if(!dbPointer)
        dbPointer = app().getDbClient();

    orm::Mapper<models::Genres> mp(dbPointer);
    models::Genres genre;
    genre.setOrigin(enumToInt(provider));
    genre.setExternalId(extID);
    try
    {
        mp.insert(genre);
        return genre.getPrimaryKey();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
}

void insertGenre(const int64_t mediaItemID, const MetaDataProvider provider, const std::string& extID, const std::string& name, const Language language, orm::DbClientPtr dbPointer)
{
    LOG_DEBUG<<std::format("insertGenre({}, {}, {}, {}, {})", mediaItemID, enumToInt(provider), extID , name, enumToInt(language));
    if(!dbPointer)
        dbPointer = app().getDbClient();
    std::optional<int64_t> genreID = findGenre(provider, extID, dbPointer);
    if (!genreID.has_value())
    {
        genreID = insertGenre(provider, extID, dbPointer);
        if (!genreID.has_value())
        {
            return;
        }
    }
    auto future = metaDataThreadPool.submit_task([&language, &name, &genreID]{
        insertGenreLocalization(*genreID, language, name);
    });
    metaDataThreadPool.detach_task([genreID, mediaItemID, dbPointer]{assignGenre(*genreID, mediaItemID, dbPointer);});

    future.get();
    return;
}

void insertGenreLocalization(const int64_t genreID, const Language language, const std::string& name, orm::DbClientPtr dbPointer)
{
    if(!dbPointer)
        dbPointer = app().getDbClient();
    if(isGenreLocalized(genreID, language, dbPointer))
        return;
    orm::Mapper<models::GenreLocalizations> mp(dbPointer);
    models::GenreLocalizations genreLoc;
    genreLoc.setGenreId(genreID);
    genreLoc.setLanguageId(enumToInt(language));
    genreLoc.setName(name);
    try
    {
        mp.insert(genreLoc);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
    }
}

bool assignGenre(const int64_t genreID, const int64_t mediaItemID, orm::DbClientPtr dbPointer)
{
    if(!dbPointer)
        dbPointer = app().getDbClient();
    if (isGenreAssigned(genreID, mediaItemID, dbPointer))
        return true;
    orm::Mapper<models::GenreAssignments> mp(dbPointer);
    models::GenreAssignments genreAssignment;
    genreAssignment.setGenreId(genreID);
    genreAssignment.setMediaItemId(mediaItemID);
    try
    {
        mp.insert(genreAssignment);
        return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
}

bool isGenreAssigned(const int64_t genreID, const int64_t mediaItemID, orm::DbClientPtr dbPointer)
{
    if(!dbPointer)
        dbPointer = app().getDbClient();
    return recordExists<models::GenreAssignments>(orm::Criteria(models::GenreAssignments::Cols::_genre_id, orm::CompareOperator::EQ, genreID)
     && orm::Criteria(models::GenreAssignments::Cols::_media_item_id, orm::CompareOperator::EQ, mediaItemID), dbPointer);
}

bool getTVEpisodeMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem)
{
    if (scanSettings.metaDataProvider == MetaDataProvider::Local)
        return true;
    bool parsed = false;
    std::string extID{};
    auto extIDs = getShowExternalIDFromSeason(mediaItem.getValueOfParentId());
    if (extIDs.empty())
        return parsed;
    if (hasFlag(scanSettings.metaDataProvider, MetaDataProvider::TMDB) && extIDInVec(extIDs, MetaDataProvider::TMDB, extID))
    {
        parsed = getTVEpisodeMetaDataTMDB(scanSettings, mediaItem, std::stoi(extID));
    }
    return parsed;
}

bool getMovieMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem)
{
    if (scanSettings.metaDataProvider == MetaDataProvider::Local)
        return true;
    bool parsed = false;
    if (hasFlag(scanSettings.metaDataProvider, MetaDataProvider::TMDB))
    {
        LOG_INFO<<std::format("Ищем фильм в TMDB {}", mediaItem.getValueOfParsedName());
        parsed = getMovieMetaDataTMDB(scanSettings, mediaItem);
    }
    return parsed;
}

std::optional<int64_t> insertMediaItemLocalization(const int64_t mediaItemID, const Language language, const std::string& name, const std::string& description, const std::string& tagline, const bool original)
{
    LOG_DEBUG<<std::format("insertMediaItemLocalization({}, {}, {}, {}, {})", mediaItemID, enumToInt(language), name, description, tagline);
    auto dbPointer = drogon::app().getDbClient();
    orm::Mapper<models::MediaItemLocalizations> mp(dbPointer);
    std::optional<models::MediaItemLocalizations> localization;
    localization = findMediaItemLocalization(mediaItemID, language, dbPointer);
    if (localization.has_value() && localization->getValueOfName() != name && localization->getValueOfDescription() != description && localization->getValueOfTagline() != tagline)
    {
        // localization->setMediaItemId(mediaItemID);
        localization->setName(name);
        localization->setDescription(description);
        localization->setTagline(tagline);
        try
        {
            mp.insert(*localization);
            return localization->getPrimaryKey();
        }
        catch (const drogon::orm::DrogonDbException& e)
        {
            LOG_ERROR<<"Ошибка запроса обновления: "<<e.base().what();
            return {};
        }
    }
    else 
    {
        models::MediaItemLocalizations loc;
        loc.setMediaItemId(mediaItemID);
        loc.setName(name);
        loc.setDescription(description);
        loc.setTagline(tagline);
        loc.setLanguageId(enumToInt(language));
        loc.setOriginal(original);
        try
        {
            mp.insert(loc);
            return loc.getPrimaryKey();
        }
        catch (const drogon::orm::DrogonDbException& e)
        {
            LOG_ERROR<<"Ошибка запроса вставки: "<<e.base().what();
            return {};
        }
    }
    updateMediaItemTimestamp(mediaItemID);
}

std::optional<models::MediaItemLocalizations> findMediaItemLocalization(const int64_t mediaItemID, const Language language, orm::DbClientPtr dbPointer)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    orm::Mapper<models::MediaItemLocalizations> mp(dbPointer);
    try
    {
        return mp.findOne(orm::Criteria(models::MediaItemLocalizations::Cols::_media_item_id, orm::CompareOperator::EQ, mediaItemID)&&
                            orm::Criteria(models::MediaItemLocalizations::Cols::_language_id, orm::CompareOperator::EQ, enumToInt(language)));
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
}   

std::optional<int64_t> insertMediaItemExternalID(const int64_t mediaItemID, const MetaDataProvider provider, const std::string& externalID)
{
    LOG_DEBUG<<std::format("insertMediaItemExternalID({}, {}, {})", mediaItemID, enumToInt(provider), externalID);
    if (externalID.empty())
        return {};
    models::ExternalMediaItemIds extMediaItemIDs;
    orm::Mapper<models::ExternalMediaItemIds> mp(drogon::app().getDbClient());
    extMediaItemIDs.setMediaItemId(mediaItemID);
    extMediaItemIDs.setMetadataProviderId(enumToInt(provider));
    extMediaItemIDs.setExternalId(externalID);
    try
    {
        mp.insert(extMediaItemIDs);
        updateMediaItemTimestamp(mediaItemID);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Не удалось вставить внешний id: "<<e.base().what();
        return {};
    }
    return {};
}

std::optional<int64_t> insertPerson(const std::string& dateOfBirth)
{
    // if (externalID.empty())
    //     return {};
    auto dbPointer = drogon::app().getDbClient();
    models::People person;
    orm::Mapper<models::People> mp(dbPointer);
    person.setDateOfBirth(dateOfBirth);
    try
    {
        mp.insert(person);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
    return {};
}

std::optional<int64_t> insertCredit(const int64_t personID, const CreditType creditType)
{
    auto dbPointer = drogon::app().getDbClient();
    return {};
}

std::optional<int64_t> insertCreditLocalization(const int64_t creditID, const Language language, const std::string& text)
{
    if (isCreditLocalized(creditID, language))
        return {};
    orm::Mapper<models::CreditLocalizations> mp(drogon::app().getDbClient());
    try
    {
        models::CreditLocalizations creditLoc;
        creditLoc.setCreditId(creditID);
        creditLoc.setLanguageId(enumToInt(language));
        creditLoc.setText(text);
        mp.insert(creditLoc);
        return creditLoc.getPrimaryKey();
    }
    catch (const drogon::orm::DrogonDbException e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
    return {};
}

std::optional<int64_t> insertPersonExternalID(const int64_t personID, const MetaDataProvider provider, const std::string& externalID)
{
    LOG_DEBUG<<std::format("insertPersonExternalID({}, {}, {})", personID, enumToInt(provider), externalID);
    if (externalID.empty())
        return {};
    models::ExternalPeopleIds extPersonIDs;
    extPersonIDs.setPersonId(personID);
    extPersonIDs.setMetadataProviderId(enumToInt(provider));
    extPersonIDs.setExternalId(externalID);
    return insertRecord(extPersonIDs);
}

std::optional<int64_t> findPerson(const MetaDataProvider provider, const std::string& externalID, orm::DbClientPtr dbPointer)
{
    LOG_DEBUG<<std::format("findPerson({}, {})", enumToInt(provider), externalID);
    if (!dbPointer)
        dbPointer = app().getDbClient();
    
    try
    {
        auto result = dbPointer->execSqlSync("select people.id as id from people inner join external_people_ids extID on (extID.person_id = people.id and extID.metadata_provider_id = $1 and extID.external_id = $2)",
        enumToInt(provider), externalID);    
        if (result.empty())
            return {};
        return result[0]["id"].as<int64_t>();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Не удалось найти человека: "<<e.base().what();
        return {};
    }
    return {};
}


std::optional<int64_t> findPerson(const std::string& name, const Gender gender, orm::DbClientPtr dbPointer)
{
    if (name.empty())
        return {};

    if (!dbPointer)
        dbPointer = drogon::app().getDbClient();
    
    try
    {
        // ПОменять
        auto result = dbPointer->execSqlSync("select person.id as id from people as person inner join people_localizations as loc where (loc.name = $1 and person.gender_id = $2 )limit 1", name, enumToInt(gender));

        if (result.empty())
            return {};

        return result[0]["id"].as<int64_t>();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
    return {};
}

std::optional<int64_t> findCredit(const std::string& role, const CreditType creditType)
{
    auto dbPointer = drogon::app().getDbClient();
    try 
    {
        
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
    return {};
}


std::optional<int64_t> findOrInsertPerson(const MetaDataProvider provider, const std::string& id, const Language language, const std::string& name)
{
    LOG_DEBUG<<std::format("findOrInsertPerson({}, {}, {}, {})", enumToInt(provider), id, enumToInt(language), name);
    std::lock_guard lockFindOrInsertPerson(findOrInsertPersonMutex);
    auto dbPointer = app().getDbClient();
    auto personID = findPerson(provider, id, dbPointer);
    if (personID.has_value())
    {
        if (hasFlag(provider, MetaDataProvider::TMDB))
            metaDataThreadPool.detach_task([personID, id, language]{localizePersonTMDB(*personID, std::stoi(id), language);});
        return personID;
    }
    return insertPerson(provider, id, language);
}

std::optional<int64_t> insertPerson(const MetaDataProvider provider, const std::string& id, const Language language)
{
    if (id.empty())
        return {};
    switch (provider) 
    {
        case MetaDataProvider::TMDB:
            return insertPersonTMDB(std::stoi(id), language);
        case MetaDataProvider::KinoPoisk:
            return {};
        case MetaDataProvider::OMDB:
            return {};
        default:
            return {};
    }
}

std::optional<int64_t> insertPersonTMDB(const int id, const Language language)
{
    LOG_INFO<<std::format("insertPersonTMDB({})", id);
    TMDBAPI::Endpoints::People people(id);
    auto details = people.getDetails(languageToTMDBLanguage(language), TMDBAPI::PeopleAppendToResponse::ExternalIds);
    if (!details.has_value())
    {
        LOG_ERROR<<std::format("Не удалось выполнить запрос на получение человека с id({}) из TMDB API", id);
        return {};
    }
    orm::Mapper<models::People> mp(app().getDbClient());
    try
    {
        models::People person;
        person.setGenderId(enumToInt(details->details.gender));
        person.setDateOfBirth(details->details.birthday);
        person.setDateOfDeath(details->details.deathday);
        //person.setOriginalName(details->name);
        if (!details->details.profilePath.empty())
        {
            auto imageID = CreateAndInsertImage(TMDBAPI::secureImageURL + "/" +details->details.profilePath, "/home/demonjarl/Studies/Diplom/MediaServer/images", ImageType::Portrait, language);
            if (imageID)
                person.setPortretId(*imageID);
        }
        mp.insert(person);
        int64_t ret = person.getPrimaryKey();
        // details->details.profilePath
        insertPersonLocalization(ret, details->details.name, details->details.biography, language);

        // threadPool.detach_task([ret, id]{
        //     insertPersonExternalID(ret, MetaDataProvider::TMDB, std::to_string(id));
        // });
 
        if (details->externalIDs.has_value())
            updateExtIds(*details->externalIDs, ret);
        // threadPool.detach_task([id, ret]{
        //     localizePersonTMDB(ret, id);
        // });
        return ret;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка вставки человека: "<<e.base().what();
        return {};
    }
}


std::optional<int64_t> insertPersonLocalization(const int64_t personID, const std::string& name, const std::string& biography, const Language language)
{
    LOG_DEBUG<<std::format("insertPersonLocalization()", personID, name, biography, enumToInt(language));
    if (name.empty())
        return {};
    try
    {
        orm::Mapper<models::PeopleLocalizations> mp(app().getDbClient());
        // if (isPersonLocalized(personID, language))
        //     return {};
        auto personLoc = findPersonLocalizationORM(personID, language);
        if (personLoc.has_value())
        {
            bool diffName = personLoc->getValueOfName() != name;
            bool diffBio = personLoc->getValueOfBiography() != biography;
            if (!diffName && !diffBio)
                return personLoc->getPrimaryKey();
            if (diffName)
                personLoc->setName(name);
            if (diffBio)
                personLoc->setBiography(biography);
            mp.update(*personLoc);
            return personLoc->getPrimaryKey();
        }
        models::PeopleLocalizations tmp;
        //personLoc.reset();
        //personLoc = models::PeopleLocalizations();
        tmp.setPersonId(personID);
        tmp.setName(name);
        tmp.setBiography(biography);
        tmp.setLanguageId(enumToInt(language));
        mp.insert(tmp);
        return tmp.getPrimaryKey();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Не удалось локализировать человека: "<<e.base().what();
        return {};
    }
}

void localizePersonTMDB(const int64_t personID, const int id, const Language language)
{
    LOG_DEBUG<<std::format("localizePersonTMDB({}, {}, {})", personID, id, enumToInt(language));
    auto dbPointer = app().getDbClient();
    
    if (findPersonLocalizationORM(personID, language))
        return;
    
    auto personDetails = TMDBAPI::Endpoints::People(id).getDetails(languageToTMDBLanguage(language));
    if (!personDetails.has_value())
    {
        LOG_ERROR<<std::format("personDetails error {}", enumToInt(personDetails.error()));
        return;
    }
    models::PeopleLocalizations loc;
    orm::Mapper<models::PeopleLocalizations> mp(dbPointer);
    loc.setLanguageId(enumToInt(language));
    loc.setPersonId(personID);
    loc.setName(personDetails->details.name);
    loc.setBiography(personDetails->details.biography);
    insertRecord(loc, mp);
}

std::optional<models::PeopleLocalizations> findPersonLocalizationORM(const int64_t personID, const Language language)
{
    orm::Mapper<models::PeopleLocalizations> mp(app().getDbClient());
    try
    {
        return findRecordByCriteriaORM<models::PeopleLocalizations>(orm::Criteria(models::PeopleLocalizations::Cols::_person_id, orm::CompareOperator::EQ, personID) &&
                orm::Criteria(models::PeopleLocalizations::Cols::_language_id, orm::CompareOperator::EQ, enumToInt(language)));
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
}

void assignCredit(const int64_t creditID, const int64_t mediaItemID)
{
    models::CreditAssignments assignment;
    assignment.setCreditId(creditID);
    assignment.setMediaItemId(mediaItemID);
    insertRecord(assignment);
}

std::optional<int64_t> insertCredit(const int64_t personID, const CreditType creditType, const MetaDataProvider origin, const std::string& externalID)
{
    models::Credits credit;
    credit.setPersonId(personID);
    credit.setCreditTypeId(enumToInt(creditType));
    credit.setOrigin(enumToInt(origin));
    credit.setExternalId(externalID);
    return insertRecord(credit);
}

std::optional<int64_t> insertCredit(const int64_t personID, const int64_t mediaItemID, const CreditType creditType, const MetaDataProvider origin, const std::string& externalID)
{
    models::Credits credit;
    credit.setPersonId(personID);
    credit.setCreditTypeId(enumToInt(creditType));
    credit.setOrigin(enumToInt(origin));
    credit.setExternalId(externalID);
    auto ret = insertRecord(credit);
    if (!ret.has_value())
        return {};
    assignCredit(*ret, mediaItemID);
    return ret;
}

std::optional<int64_t> findOrInsertCredit(const int64_t mediaItemID, const int64_t personID, const CreditType type, const MetaDataProvider origin, const std::string& externalID)
{
    LOG_DEBUG<<std::format("findOrInsertCredit({}, {}, {}, {}, {})", mediaItemID, personID, enumToInt(type), enumToInt(origin), externalID);
    std::lock_guard lock(findOrInsertCreditMutex);
    auto creditID = findCredit(origin, externalID);
    if (creditID.has_value())
    {
        assignCredit(*creditID, mediaItemID);
        return creditID;
    }
    return insertCredit(personID, mediaItemID, type, origin, externalID);
}


void insertCredits(const std::vector<TMDBAPI::Models::CrewCredit>& crewCredits, const Language language, const int64_t mediaItemID)
{

    if (crewCredits.empty())
        return;

    std::for_each(crewCredits.begin(), crewCredits.end(), [mediaItemID, language](auto& crew){
        auto personID = findOrInsertPerson(MetaDataProvider::TMDB, std::to_string(crew.id), language, crew.name);
        if (!personID.has_value())
        {
            LOG_ERROR<<std::format("Не удалось найти или вставить человека с ID {}", crew.id);
            return; 
        }
        auto creditID = findOrInsertCredit(mediaItemID, *personID, CreditType::Production, MetaDataProvider::TMDB, crew.creditID);

        if (!creditID.has_value())
        {
            LOG_INFO<<std::format("Не удалось найти или вставить credit {}", crew.creditID);
            return;
        }

        insertCreditLocalization(*creditID, language, crew.job);
        // for (const auto& job : crew.jobs)
        //     insertCreditLocalization(*creditID, language, job.value);
    });
    // auto futureCrew = threadPool.submit_loop(0, crewCredits.size(), [&crewCredits, mediaItemID, language](size_t ind){
    //     const auto& crew = crewCredits[ind];
    //     auto personID = findOrInsertPerson(MetaDataProvider::TMDB, std::to_string(crew.id), language, crew.name);
    //     if (!personID.has_value())
    //     {
    //         LOG_ERROR<<std::format("Не удалось найти или вставить человека с ID {}", crew.id);
    //         return; 
    //     }
    //     auto creditID = findCredit(*personID, mediaItemID, CreditType::Production);
    //     // for (const auto& job : crew.jobs)
    //     // {
            
    //     // }
    //     // Проверять если локализации?
    //     if (!creditID.has_value())
    //         creditID = insertCredit(*personID, mediaItemID, CreditType::Production);
        
    //     if (!creditID.has_value())
    //     {
    //         // лог
    //         LOG_ERROR<<std::format("Не удалось вставить Credit");
    //         return;
    //     }
    //     insertCreditLocalization(*creditID, language, crew.job);
    //     // for (const auto& job : crew.jobs)
    //     //     insertCreditLocalization(*creditID, language, job.value);
    // });
    // futureCrew.wait();
    return;
}



void insertCredits(const std::vector<TMDBAPI::Models::CastCredit>& castCredits, const Language language, const int64_t mediaItemID)
{

    if (castCredits.empty())
        return;

    std::for_each(castCredits.begin(), castCredits.end(), [mediaItemID, language](auto& cast){
        auto personID = findOrInsertPerson(MetaDataProvider::TMDB, std::to_string(cast.id), language, cast.name);
        if (!personID.has_value())
        {
            LOG_ERROR<<std::format("Не удалось найти или вставить человека с ID {}", cast.id);
            return; 
        }
        auto creditID = findOrInsertCredit(mediaItemID, *personID, CreditType::Actor, MetaDataProvider::TMDB, cast.creditID);
       
        if (!creditID.has_value())
        {
            LOG_INFO<<std::format("Не удалось найти или вставить credit {}", cast.creditID);
            return;
        }
        
        insertCreditLocalization(*creditID, language, cast.character);
    });
    // auto futureCast = threadPool.submit_loop(0, castCredits.size(), [&castCredits, mediaItemID, language](size_t ind){
    //     const auto& cast = castCredits[ind];
    //     auto personID = findOrInsertPerson(MetaDataProvider::TMDB, std::to_string(cast.id), language, cast.name);
    //     if (!personID.has_value())
    //     {
    //         LOG_ERROR<<std::format("Не удалось найти или вставить человека с ID {}", cast.id);
    //         return; 
    //     }
    //     auto creditID = findCredit(*personID, mediaItemID, CreditType::Actor);
    //     // Проверять если локализации?
    //     if (!creditID.has_value())
    //         creditID = insertCredit(*personID, mediaItemID, CreditType::Actor);
        
        
    //     if (!creditID.has_value())
    //     {
    //         // лог
    //         LOG_ERROR<<std::format("Не удалось вставить Credit");
    //         return;
    //     }   
    //     insertCreditLocalization(*creditID, language, cast.character);
    //     // LOG_DEBUG<<std::format("role size {}",cast.roles.size());
    //     // for (const auto& role : cast.roles)
    //     // {
    //     //     LOG_DEBUG<<std::format("role[{}] {}", role.creditId, role.value);
    //     //     insertCreditLocalization(*creditID, language, role.value);
    //     // }
    // });
    // futureCast.get();
    return;
}

std::optional<int64_t> findGenre(const std::string& name)
{
    orm::Mapper<models::GenreLocalizations> mp(app().getDbClient());
    try
    {
        auto genreLoc = mp.findBy(orm::Criteria(models::GenreLocalizations::Cols::_name, orm::CompareOperator::EQ, name));
        if (genreLoc.empty())
            return {};
        return genreLoc[0].getValueOfGenreId();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
}
std::optional<int64_t> insertGenre()
{
    orm::Mapper<models::Genres> mp(app().getDbClient());
    try
    {
        models::Genres genre;
        mp.insert(genre);
        return genre.getValueOfId();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
    return {};
}
std::optional<int64_t> insertGenreLocalization(const int64_t genreID, const Language language, const std::string& name)
{
    orm::Mapper<models::GenreLocalizations> mp(app().getDbClient());
    try
    {
        models::GenreLocalizations genreLoc;
        genreLoc.setGenreId(genreID);
        genreLoc.setLanguageId(enumToInt(language));
        genreLoc.setName(name);
        mp.insert(genreLoc);
        return genreLoc.getPrimaryKey();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
}
bool assignGenre(const int64_t genreID, const int64_t mediaItemID)
{
    orm::Mapper<models::GenreAssignments> mp(app().getDbClient());

    try
    {
        models::GenreAssignments assignment;
        assignment.setGenreId(genreID);
        assignment.setMediaItemId(mediaItemID);
        mp.insert(assignment);
        updateMediaItemTimestamp(mediaItemID);
        return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
}
bool isAssigned(const int64_t genreID, const int64_t mediaItemID)
{
    orm::Mapper<models::GenreAssignments> mp(app().getDbClient());
    try
    {
        auto assignment = mp.findOne(orm::Criteria(models::GenreAssignments::Cols::_genre_id, orm::CompareOperator::EQ, genreID) &&
                        orm::Criteria(models::GenreAssignments::Cols::_media_item_id, orm::CompareOperator::EQ, mediaItemID));
        return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
}


// Возвращает response.text
std::string getImageData(const std::string& imageLink)
{
    cpr::Response resp = cpr::Get(cpr::Url(imageLink));
    if (resp.error.code != cpr::ErrorCode::OK)
    {
        LOG_ERROR<<std::format("getImageData cpr error {}", resp.error.message);
        return {};
    }

    if (resp.status_code != 200)
    {
        LOG_ERROR<<std::format("getImageData error {}", resp.text);
        return {};
    }
    std::string contentType = resp.header["content-type"];
    if (contentType.find("image") == std::string::npos)
    {
        LOG_ERROR<<std::format("Ошибка тип контента не изображение, а {}", contentType);
        return {};
    }
    return std::move(resp.text);
}

// Возвращает путь на файл
std::string createImageFromLink(const std::string& imageLink, const std::string& pathToDirectory)
{
    LOG_DEBUG<<std::format("createImageFromLink({}, {})", imageLink, pathToDirectory);
    if (imageLink.empty() || pathToDirectory.empty())
        return {};
    std::string filePath = pathToDirectory + ((imageLink[0] == '/') ? "" : "/") + imageLink.substr(imageLink.rfind("/")+1);
    if (pathExists(filePath))
    {
        LOG_INFO<<"Файл уже существует";
        return {};
    }
    LOG_INFO<<std::format("filePath {}", filePath);
    std::string imageData = getImageData(imageLink);
    if (imageData.empty())
        return {};
    std::ofstream imageFile(filePath, std::ios::binary | std::ios::out);
    imageFile << imageData;
    imageFile.close();
    return filePath;
}

std::optional<int64_t> CreateAndInsertImage(const std::string& imageLink, const std::string& pathToDirectory, const ImageType type, const Language language)
{
    std::string imagePath = createImageFromLink(imageLink, pathToDirectory);
    if (imagePath.empty())
        return {};
    return insertImage(imagePath, type, language);
}


std::optional<int64_t> insertImage(const std::string& path, const ImageType type, const Language language)
{
    if (path.empty())
        return {};
    models::Images image;
    image.setLanguageId(enumToInt(language));
    image.setImageTypeId(enumToInt(type));
    image.setPath(path);
    if (insertRecord(image))
        return image.getPrimaryKey();
    return {};
}

void assignImage(const int64_t imageID, const int64_t mediaItemID)
{
    models::MediaItemImageAssignments assignment;
    assignment.setMediaItemId(mediaItemID);
    assignment.setImageId(imageID);
    insertRecord(assignment);
    return;
}

void insertImage(const ImageType type, const Language language, const std::string& path, const int64_t mediaItemID)
{
    if (path.empty())
        return;
    auto imageID = insertImage(path, type, language);
    if (!imageID.has_value())
        return;
    assignImage(*imageID, mediaItemID);
}

void insertImage(const LibraryScanSettings& scanSettings, const ImageType type, const Language language, const std::string& imageLink, const int64_t mediaItemID)
{
    std::string saveDirectory{"/home/demonjarl/Studies/Diplom/MediaServer/images"};// scanSettings
    auto imageID = CreateAndInsertImage(imageLink, saveDirectory, type, language);
    if (!imageID.has_value())
        return;
    assignImage(*imageID, mediaItemID);
}

void insertImages(const TMDBAPI::Models::PartialMediaImages& images, const Language& language, const int64_t mediaItemID)
{

}



std::optional<int64_t> insertProductionCompany(const std::string& name, orm::DbClientPtr dbPointer)
{
    if(!dbPointer)
        dbPointer = app().getDbClient();

    auto rec = findRecordByCriteriaORM<models::ProductionCompanies>(orm::Criteria(models::ProductionCompanies::Cols::_name, orm::CompareOperator::EQ, name), dbPointer);
    if (rec.has_value())
    {
        return rec->getPrimaryKey();
    }
    try
    {
        orm::Mapper<models::ProductionCompanies> mp(dbPointer);
        models::ProductionCompanies company;
        company.setName(name);
        mp.insert(company);
        return company.getPrimaryKey();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return {};
    }
}

bool assignProductionCompany(const int64_t mediaItemID, const int64_t productionCompanyID, orm::DbClientPtr dbPointer)
{
    if(!dbPointer)
        dbPointer = app().getDbClient();
    try
    {
        if (recordExists<models::ProductionCompanyAssignments>(orm::Criteria(models::ProductionCompanyAssignments::Cols::_media_item_id, orm::CompareOperator::EQ, mediaItemID)
            && orm::Criteria(models::ProductionCompanyAssignments::Cols::_production_company_id, orm::CompareOperator::EQ, productionCompanyID), dbPointer))
            return true;
        orm::Mapper<models::ProductionCompanyAssignments> mp(dbPointer);
        models::ProductionCompanyAssignments assignment;
        assignment.setProductionCompanyId(productionCompanyID);
        assignment.setMediaItemId(mediaItemID);
        mp.insert(assignment);
        updateMediaItemTimestamp(mediaItemID);
        return true;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
}
// Вставка/нахождение компании и привязка
std::optional<int64_t> insertProductionCompany(const int64_t mediaItemID, const std::string& name)
{
    auto dbPointer = app().getDbClient();
    auto companyID = insertProductionCompany(name, dbPointer);
    if (!companyID.has_value())
        return {};
    metaDataThreadPool.detach_task([mediaItemID, companyID, dbPointer]{
        assignProductionCompany(mediaItemID, *companyID, dbPointer);
    });
    return companyID;
}



TMDBAPI::Languages languageToTMDBLanguage(const Language language)
{
    switch (language) {

    case Language::en:
        return TMDBAPI::Languages::En;
    case Language::aa:
        return TMDBAPI::Languages::Aa;
    case Language::ab:
        return TMDBAPI::Languages::Ab;
    case Language::af:
        return TMDBAPI::Languages::Af;
    case Language::am:
        return TMDBAPI::Languages::Am;
    case Language::ar:
        return TMDBAPI::Languages::Ar;
    case Language::as:
        return TMDBAPI::Languages::As;
    case Language::ay:
        return TMDBAPI::Languages::Ay;
    case Language::az:
        return TMDBAPI::Languages::Az;
    case Language::ba:
        return TMDBAPI::Languages::Ba;
    case Language::be:
        return TMDBAPI::Languages::Be;
    case Language::bg:
        return TMDBAPI::Languages::Bg;
    case Language::bi:
        return TMDBAPI::Languages::Bi;
    case Language::bn:
        return TMDBAPI::Languages::Bn;
    case Language::bo:
        return TMDBAPI::Languages::Bo;
    case Language::br:
        return TMDBAPI::Languages::Br;
    case Language::ca:
        return TMDBAPI::Languages::Ca;
    case Language::co:
        return TMDBAPI::Languages::Co;
    case Language::cs:
        return TMDBAPI::Languages::Cs;
    case Language::cy:
        return TMDBAPI::Languages::Cy;
    case Language::da:
        return TMDBAPI::Languages::Da;
    case Language::de:
        return TMDBAPI::Languages::De;
    case Language::dz:
        return TMDBAPI::Languages::Dz;
    case Language::el:
        return TMDBAPI::Languages::El;
    case Language::eo:
        return TMDBAPI::Languages::Eo;
    case Language::es:
        return TMDBAPI::Languages::Es;
    case Language::et:
        return TMDBAPI::Languages::Et;
    case Language::eu:
        return TMDBAPI::Languages::Eu;
    case Language::fa:
        return TMDBAPI::Languages::Fa;
    case Language::fi:
        return TMDBAPI::Languages::Fi;
    case Language::fj:
        return TMDBAPI::Languages::Fj;
    case Language::fo:
        return TMDBAPI::Languages::Fo;
    case Language::fr:
        return TMDBAPI::Languages::Fr;
    case Language::fy:
        return TMDBAPI::Languages::Fy;
    case Language::ga:
        return TMDBAPI::Languages::Ga;
    case Language::gd:
        return TMDBAPI::Languages::Gd;
    case Language::gl:
        return TMDBAPI::Languages::Gl;
    case Language::gn:
        return TMDBAPI::Languages::Gn;
    case Language::gu:
        return TMDBAPI::Languages::Gu;
    case Language::ha:
        return TMDBAPI::Languages::Ha;
    case Language::hi:
        return TMDBAPI::Languages::Hi;
    case Language::hr:
        return TMDBAPI::Languages::Hr;
    case Language::hu:
        return TMDBAPI::Languages::Hu;
    case Language::hy:
        return TMDBAPI::Languages::Hy;
    case Language::ia:
        return TMDBAPI::Languages::Ia;
    case Language::ie:
        return TMDBAPI::Languages::Ie;
    case Language::ik:
        return TMDBAPI::Languages::Ik;
    case Language::in:
        return TMDBAPI::Languages::Id;
    case Language::is:
        return TMDBAPI::Languages::Is;
    case Language::it:
        return TMDBAPI::Languages::It;
    case Language::iw:
        return TMDBAPI::Languages::He;
    case Language::ja:
        return TMDBAPI::Languages::Ja;
    case Language::ji:
        return TMDBAPI::Languages::Yi;
    case Language::jw:
        return TMDBAPI::Languages::Jv;
    case Language::ka:
        return TMDBAPI::Languages::Ka;
    case Language::kk:
        return TMDBAPI::Languages::Kk;
    case Language::kl:
        return TMDBAPI::Languages::Kl;
    case Language::km:
        return TMDBAPI::Languages::Km;
    case Language::kn:
        return TMDBAPI::Languages::Kn;
    case Language::ko:
        return TMDBAPI::Languages::Ko;
    case Language::ks:
        return TMDBAPI::Languages::Ks;
    case Language::ku:
        return TMDBAPI::Languages::Ku;
    case Language::ky:
        return TMDBAPI::Languages::Ky;
    case Language::la:
        return TMDBAPI::Languages::La;
    case Language::ln:
        return TMDBAPI::Languages::Ln;
    case Language::lo:
        return TMDBAPI::Languages::Lo;
    case Language::lt:
        return TMDBAPI::Languages::Lt;
    case Language::lv:
        return TMDBAPI::Languages::Lv;
    case Language::mg:
        return TMDBAPI::Languages::Mg;
    case Language::mi:
        return TMDBAPI::Languages::Mi;
    case Language::mk:
        return TMDBAPI::Languages::Mk;
    case Language::ml:
        return TMDBAPI::Languages::Ml;
    case Language::mn:
        return TMDBAPI::Languages::Mn;
    case Language::mo:
        return TMDBAPI::Languages::Mo;
    case Language::mr:
        return TMDBAPI::Languages::Mr;
    case Language::ms:
        return TMDBAPI::Languages::Ms;
    case Language::mt:
        return TMDBAPI::Languages::Mt;
    case Language::my:
        return TMDBAPI::Languages::My;
    case Language::na:
        return TMDBAPI::Languages::Na;
    case Language::ne:
        return TMDBAPI::Languages::Ne;
    case Language::nl:
        return TMDBAPI::Languages::Nl;
    case Language::no:
        return TMDBAPI::Languages::No;
    case Language::oc:
        return TMDBAPI::Languages::Oc;
    case Language::om:
        return TMDBAPI::Languages::Om;
    case Language::pa:
        return TMDBAPI::Languages::Pa;
    case Language::pl:
        return TMDBAPI::Languages::Pl;
    case Language::ps:
        return TMDBAPI::Languages::Ps;
    case Language::pt:
        return TMDBAPI::Languages::Pt;
    case Language::qu:
        return TMDBAPI::Languages::Qu;
    case Language::rm:
        return TMDBAPI::Languages::Rm;
    case Language::rn:
        return TMDBAPI::Languages::Rn;
    case Language::ro:
        return TMDBAPI::Languages::Ro;
    case Language::ru:
        return TMDBAPI::Languages::Ru;
    case Language::rw:
        return TMDBAPI::Languages::Rw;
    case Language::sa:
        return TMDBAPI::Languages::Sa;
    case Language::sd:
        return TMDBAPI::Languages::Sd;
    case Language::sg:
        return TMDBAPI::Languages::Sg;
    case Language::sh:
        return TMDBAPI::Languages::Sh;
    case Language::si:
        return TMDBAPI::Languages::Si;
    case Language::sk:
        return TMDBAPI::Languages::Sk;
    case Language::sl:
        return TMDBAPI::Languages::Sl;
    case Language::sm:
        return TMDBAPI::Languages::Sm;
    case Language::sn:
        return TMDBAPI::Languages::Sn;
    case Language::so:
        return TMDBAPI::Languages::So;
    case Language::sq:
        return TMDBAPI::Languages::Sq;
    case Language::sr:
        return TMDBAPI::Languages::Sr;
    case Language::ss:
        return TMDBAPI::Languages::Ss;
    case Language::st:
        return TMDBAPI::Languages::St;
    case Language::su:
        return TMDBAPI::Languages::Su;
    case Language::sv:
        return TMDBAPI::Languages::Sv;
    case Language::sw:
        return TMDBAPI::Languages::Sw;
    case Language::ta:
        return TMDBAPI::Languages::Ta;
    case Language::te:
        return TMDBAPI::Languages::Te;
    case Language::tg:
        return TMDBAPI::Languages::Tg;
    case Language::th:
        return TMDBAPI::Languages::Th;
    case Language::ti:
        return TMDBAPI::Languages::Ti;
    case Language::tk:
        return TMDBAPI::Languages::Tk;
    case Language::tl:
        return TMDBAPI::Languages::Tl;
    case Language::tn:
        return TMDBAPI::Languages::Tn;
    case Language::to:
        return TMDBAPI::Languages::To;
    case Language::tr:
        return TMDBAPI::Languages::Tr;
    case Language::ts:
        return TMDBAPI::Languages::Ts;
    case Language::tt:
        return TMDBAPI::Languages::Tt;
    case Language::tw:
        return TMDBAPI::Languages::Tw;
    case Language::uk:
        return TMDBAPI::Languages::Uk;
    case Language::ur:
        return TMDBAPI::Languages::Ur;
    case Language::uz:
        return TMDBAPI::Languages::Uz;
    case Language::vi:
        return TMDBAPI::Languages::Vi;
    case Language::vo:
        return TMDBAPI::Languages::Vo;
    case Language::wo:
        return TMDBAPI::Languages::Wo;
    case Language::xh:
        return TMDBAPI::Languages::Xh;
    case Language::yo:
        return TMDBAPI::Languages::Yo;
    case Language::zh:
        return TMDBAPI::Languages::Zh;
    case Language::zu:
        return TMDBAPI::Languages::Zu;
    case Language::xx:
        return TMDBAPI::Languages::Xx;
    //  На всякий
    default:
        return TMDBAPI::Languages::Xx;
    }
}
Language TMDBLanguageToLanguage(const TMDBAPI::Languages language)
{
    switch (language) {
        case TMDBAPI::Languages::En:
            return Language::en;
        case TMDBAPI::Languages::Ru:
            return Language::ru;
        case TMDBAPI::Languages::Ja:
            return Language::ja;
        case TMDBAPI::Languages::Ko:
            return Language::ko;
    }
    return Language::ru;
}