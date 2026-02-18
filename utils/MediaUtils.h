#pragma once
#include <cstdint>
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>
#include <filesystem>
#include <format>
#include <json/value.h>
#include <models/Libraries.h>
#include <optional>
#include <string>
#include <trantor/utils/Logger.h>
#include <vector>
#include "CastCredit.hpp"
#include "CreditLocalizations.h"
#include "Credits.h"
#include "Credits.hpp"
#include "CrewCredit.hpp"
#include "DBUtils.h"
#include "EnumToInt.h"
#include "Enums.hpp"
#include "ExternalMediaItemIds.h"
#include "ExternalPeopleIds.h"
#include "GenreAssignments.h"
#include "GenreLocalizations.h"
#include "Images.h"
#include "MediaEnums.h"
#include "MediaImages.hpp"
#include "MediaItemLocalizations.h"
#include "MediaItemStreams.h"
#include "MediaItems.h"
#include "MovieExternalIds.hpp"
#include "PartialMediaImages.hpp"
#include "People.h"
#include "PeopleLocalizations.h"
#include "MetaDataAPI/TMDB/Models/AggregateCrewCredit.hpp"
#include "MetaDataAPI/TMDB/Models/AggregateCastCredit.hpp"
#include "MetaDataAPI/TMDB/Models/TVSeriesAggregateCredits.hpp"
#include "PersonExternalIds.hpp"
#include "ProductionCompanies.h"
#include "LibrarySettingsManager.hpp"
#include "TVEpisodeExternalIds.hpp"
#include "TVSeasonExternalIds.hpp"
#include "TVSeriesExternalIds.hpp"
#include "TimeUtils.h"
#include "Utils.h"
#include "cpr/cprtypes.h"
namespace models = drogon_model::sqlite3;
namespace fs = std::filesystem;
// Подумать над llfio https://github.com/ned14/llfio

using namespace drogon;

struct AVFormatContext;
struct AVStream;

struct LibraryScanSettings
{
    std::string libraryId;
    MetaDataProvider metaDataProvider = MetaDataProvider::Local;//= MetaDataProvider::TMDB;
    bool includeAdult = false;
    bool collectEpisodeCredits = false;
    bool collectSeasonCredits = false;
    bool collectMovieCredits = false;
    bool collectShowCredits = false;
    std::vector<Language> languagesToScanFor{Language::en, Language::ru};
    //bool includeMedia{};
    LibraryScanSettings(){};
    LibraryScanSettings(const Json::Value& librarySettings)
    {
        metaDataProvider = static_cast<MetaDataProvider>(librarySettings[LibrarySettingsManager::Fields::metadataProviders].asInt64());
    }
    LibraryScanSettings(const models::Libraries& library)
    {
        Json::Value librarySettings;
        Json::Reader reader;
        reader.parse( library.getValueOfSettings(), librarySettings );
        metaDataProvider = static_cast<MetaDataProvider>(librarySettings[LibrarySettingsManager::Fields::metadataProviders].asInt64());
    }
};

void scanLibrary(const int64_t, ScanMode mode, orm::DbClientPtr dbPointer = nullptr);
void scanLibrary(const models::Libraries& library, ScanMode mode, orm::DbClientPtr dbPointer = nullptr);
void scanLibraries(ScanMode mode, orm::DbClientPtr dbPointer = nullptr);
void scanDirectory(fs::directory_entry directory);
void analyzeFile(const fs::directory_entry& file, const LibraryScanSettings& scanSettings, const int64_t pathId);
void scanPath(const fs::path& path, const std::vector<std::string>& scanedPaths, const LibraryScanSettings& scanSettings, const int64_t pathId);
std::vector<models::MediaItemStreams> createStreams(AVFormatContext* ctx);
std::optional<int64_t> insertMediaItemLocalization(const int64_t  mediaItemID, const Language language, const std::string& name, const std::string& description, const std::string& tagline, const bool original = false);
std::optional<models::MediaItemLocalizations> findMediaItemLocalization(const int64_t mediaItemID, const Language language, orm::DbClientPtr dbPointer = nullptr);
std::vector<fs::directory_entry> enumerateFiles(const std::string& path);
std::vector<fs::directory_entry> enumerateFiles(std::vector<std::string> path);

void assignMediaItemToLibraryByPath(const int64_t mediaItemID, const std::string& path);
void assignMediaItemToLibraryByID(const int64_t mediaItemID, const int64_t libraryID);


void insertStreams(AVStream** streams, uint streamCount, int64_t mediaItemID, orm::DbClientPtr dbPointer = nullptr);
models::MediaItemStreams parseStream(AVStream& stream, int64_t mediaItemID);
void parseFileName(const std::string& fileName, std::string& name, int& season, int& episode, int& year);

bool getDataFromMetadataProviders(const LibraryScanSettings& scanSettings, const models::MediaItems& mediaItem);

std::vector<models::ExternalMediaItemIds> getMediaItemExternalID(const models::MediaItems& mediaItem, orm::DbClientPtr dbPointer = nullptr);
std::vector<models::ExternalMediaItemIds> getMediaItemExternalID(const int64_t mediaItemID, orm::DbClientPtr dbPointer = nullptr);
std::vector<models::ExternalMediaItemIds> getShowExternalIDFromSeason(const int64_t seasonMediaItemID, orm::DbClientPtr dbPointer = nullptr);

bool getTVShowMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);
bool getTVSeasonMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem, const std::vector<models::ExternalMediaItemIds>& showExternalIDs);
bool getTVEpisodeMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);
bool getMovieMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);

bool getTVSeasonMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);
bool getTVEpisodeMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem, const int TMDBShowID);
bool getMovieMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);
bool getTVShowMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);

bool getMovieMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBid, const int64_t mediaItemID, const Language language, const bool original = false);
bool getTVSeasonMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBShowID, const int TMDBSeasonID, const int64_t mediaItemID, const Language language, const bool original = false);
bool getTVShowMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBid, const int64_t mediaItemID, const Language language, const bool original = false);
bool getTVEpisodeMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBShowID, const int season, const int episode, const int64_t mediaItemID, const Language language, const bool original = false);

void updateExtIds(const TMDBAPI::Models::PersonExternalIds& extIDs, const int64_t personID);
void updateExtIds(const TMDBAPI::Models::MovieExternalIds& extIDs, const int64_t mediaItemID);
void updateExtIds(const TMDBAPI::Models::TVEpisodeExternalIds& extIDs, const int64_t mediaItemID);
void updateExtIds(const TMDBAPI::Models::TVSeriesExternalIds& extIDs, const int64_t mediaItemID);
void updateExtIds(const TMDBAPI::Models::TVSeasonExternalIds& extIDs, const int64_t mediaItemID);
void updateExtIds(const TMDBAPI::Models::PersonExternalIds& extIDs, const int64_t personID);

inline Language getOriginalLanguage(const int64_t mediaItemID)
{
    auto loc = findRecordByCriteriaORM<models::MediaItemLocalizations>(orm::Criteria(models::MediaItemLocalizations::Cols::_media_item_id, orm::CompareOperator::EQ, mediaItemID)
                                                                        && orm::Criteria(models::MediaItemLocalizations::Cols::_original, orm::CompareOperator::EQ, true));
    if (!loc.has_value())
        return Language::xx;
    return static_cast<Language>(loc->getValueOfLanguageId());
}

std::optional<int64_t> getParentForEpisode(const std::string& showTitle, const int season, const std::string& releaseDate, const LibraryScanSettings& scanSettings, orm::DbClientPtr dbPointer = nullptr);

std::optional<models::MediaItems> createMediaItem(const LibraryScanSettings& scanSettings, MediaType type, int64_t parentID, const std::string& parsedTitle, const std::string& path, const int season, const int episode, const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);
std::optional<int64_t> createTVShowMediaItem(const LibraryScanSettings& scanSettings, const std::string& parsedTitle, const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);
std::optional<int64_t> createTVSeasonMediaItem(const LibraryScanSettings& scanSettings, const int64_t parentID, const int season, const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);
std::optional<int64_t> createTVEpisodeMediaItem(const LibraryScanSettings& scanSettings, const int64_t parentID, const std::string& parsedTitle, const std::string& path, const int season, const int episode,  const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);




inline void updateMediaItemTimestamp(const int64_t mediaItemId)
{
    //auto mediaItem = findRecordByPrimaryKeyORM<models::MediaItems>(mediaItemId);
    if (!execSQL("update media_items set last_updated = $1 where id = $2", getCurrentDateTime(), mediaItemId))
        LOG_ERROR<<"Не удалось обновить время последнего обновления медиа предмета"; 
}   

inline void updateMediaItemTimestamp(models::MediaItems& mediaItem, orm::DbClientPtr dbPointer)
{
    if(!dbPointer)
        dbPointer = app().getDbClient();
    mediaItem.setLastUpdated(getCurrentDateTime());
    if (!updateRecord(mediaItem, dbPointer))
        LOG_ERROR<<"Не удалось обновить время последнего обновления медиа предмета"; 
}

inline void updateMediaItemTimestamp(models::MediaItems& mediaItem, orm::Mapper<models::MediaItems>& mp)
{
    mediaItem.setLastUpdated(getCurrentDateTime());
    if (!updateRecord(mediaItem, mp))
        LOG_ERROR<<"Не удалось обновить время последнего обновления медиа предмета"; 
}

std::optional<int64_t> findSeasonMediaItemID(const std::string& seriesTitle, const int64_t season, const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);
std::optional<int64_t> findShowMediaItemID(const std::string& seriesTitle, const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);

std::optional<int64_t> findShowByExternalID(const std::vector<std::pair<std::string, MetaDataProvider>> exteranlIDs);
std::optional<int64_t> findSeasonByExternalID(const std::vector<std::pair<std::string, MetaDataProvider>> exteranlIDs);

//std::optional<int64_t> insertMediaItemLocalization(const int64_t mediaItemID, const Language language, const std::string& name, const std::string& description);
std::optional<int64_t> insertMediaItemExternalID(const int64_t mediaItemID, const MetaDataProvider provider, const std::string& externalID);
std::optional<int64_t> insertPerson(const MetaDataProvider provider, const std::string& id, const Language language);
std::optional<int64_t> insertPersonTMDB(const int id, const Language language);
std::optional<int64_t> findOrInsertPerson(const MetaDataProvider provider, const std::string& id, const Language language, const std::string& name);


std::vector<models::MediaItems> findMediaItemsByPerson(const int64_t personID, const CreditType creditType);
std::vector<models::MediaItems> findMediaItemsByProductionCompany(const int64_t productionCompanyID);
std::vector<models::MediaItems> findMediaItemsByGenre(const std::vector<int64_t>& genreID);

std::optional<models::People> findPersonORM(const std::string& name, const std::string& dateOfBirth, orm::DbClientPtr dbPointer = nullptr);
std::optional<models::People> findPersonORM(const models::ExternalPeopleIds& extID, orm::DbClientPtr dbPointer = nullptr);
std::optional<models::People> findPersonORM(const MetaDataProvider provider, const std::string& externalID, orm::DbClientPtr dbPointer = nullptr);
std::optional<models::People> findPersonORM(const std::vector<models::ExternalPeopleIds>& extIDs, orm::DbClientPtr dbPointer = nullptr);

std::optional<int64_t> findPerson(const std::string& name, const std::string& dateOfBirth, orm::DbClientPtr dbPointer = nullptr);
std::optional<int64_t> findPerson(const std::string& name, const Gender gender, orm::DbClientPtr dbPointer = nullptr);
std::optional<int64_t> findPerson(const models::ExternalPeopleIds& extID, orm::DbClientPtr dbPointer = nullptr);
std::optional<int64_t> findPerson(const MetaDataProvider provider, const std::string& externalID, orm::DbClientPtr dbPointer = nullptr);
std::optional<int64_t> findPerson(const std::vector<models::ExternalPeopleIds>& extIDs, orm::DbClientPtr dbPointer = nullptr);

std::optional<int64_t> findPeople(const std::string& name, orm::DbClientPtr dbPointer = nullptr);
std::vector<int64_t> findPeople(const CreditType type);
std::vector<int64_t> findPeople(const int64_t mediaItemID, const CreditType type);
std::vector<int64_t> findPeople(const int64_t mediaItemID);
std::vector<models::People> findPeopleORM(const CreditType type);
std::vector<models::People> findPeopleORM(const int64_t mediaItemID, const CreditType type);
std::vector<models::People> findPeopleORM(const int64_t mediaItemID);

std::optional<models::People> insertPersonORM(const std::string& dateOfBirth, orm::DbClientPtr dbPointer = nullptr);
std::optional<models::PeopleLocalizations> insertPersonLocalizationORM(int64_t personID, const std::string& name, const std::string& biography, const Language language, orm::DbClientPtr dbPointer = nullptr);
std::optional<models::ExternalPeopleIds> insertPersonExternalIDORM(const int64_t personID, const MetaDataProvider provider, const std::string& externalID);
 
std::optional<int64_t> insertPerson(const Gender gender, const std::string& dateOfBirth, orm::DbClientPtr dbPointer = nullptr);
// Проходим по списку языков необходимых для локализации и смотрим есть ли локализация, нету вставляем
void localizePersonTMDB(const int64_t personID, const int id, const Language language);
std::optional<int64_t> insertPersonLocalization(int64_t personID, const std::string& name, const std::string& biography, const Language language);
std::optional<models::PeopleLocalizations> findPersonLocalizationORM(const int64_t personID, const Language language);
//bool personHasLocalization(const int64_t personID, const Language language);
std::optional<int64_t> insertPersonExternalID(const int64_t personID, const MetaDataProvider provider, const std::string& externalID);

std::optional<int64_t> insertCreditLocalization(const int64_t creditID, const Language language, const std::string& text);

// копируем credit и локализацию с привязкой на ToMediaItemID
void copyCredit(const int64_t creditID, const int64_t ToMediaItemID);

std::optional<int64_t> insertCredit(const TMDBAPI::Models::TVSeriesAggregateCredits& credit, const Language language, const int64_t mediaItemID);


std::optional<int64_t> insertCredit(const int64_t personID, const CreditType creditType, const MetaDataProvider origin, const std::string& externalID);
std::optional<int64_t> insertCredit(const int64_t personID, const int64_t mediaItemID, const CreditType creditType, const MetaDataProvider origin, const std::string& externalID);
std::optional<int64_t> findOrInsertCredit(const int64_t mediaItemID, const int64_t personID, const CreditType type, const MetaDataProvider origin, const std::string& externalID);

void insertCredits(const std::vector<TMDBAPI::Models::CastCredit>&, const Language language, const int64_t mediaItemID);
void insertCredits(const std::vector<TMDBAPI::Models::CrewCredit>&, const Language language, const int64_t mediaItemID);

std::vector<models::Credits> findCredits(const int64_t personID);
std::vector<models::Credits> findCredits(const int64_t personID, const CreditType creditType);
std::vector<int64_t> findCredits(const std::string& role, const CreditType creditType);
std::vector<int64_t> findCredits(const std::string& role, const CreditType creditType);
std::optional<int64_t> findCredit(const int64_t personID, const int64_t mediaItemID, const CreditType creditType, const std::string& role);
std::optional<int64_t> findCredits(const int64_t personID, const int64_t mediaItemID, const CreditType creditType);
inline std::optional<int64_t> findCredit(const MetaDataProvider origin, const std::string& externalID)
{
    return findRecordByCriteria<models::Credits>(orm::Criteria(models::Credits::Cols::_origin, orm::CompareOperator::EQ, enumToInt(origin))
                                                    && orm::Criteria(models::Credits::Cols::_external_id, orm::CompareOperator::EQ, externalID));
}
inline bool isCreditLocalized(const int64_t creditID, const Language language)
{
    return findRecordByCriteria<models::CreditLocalizations>(orm::Criteria(models::CreditLocalizations::Cols::_credit_id, orm::CompareOperator::EQ, creditID)
                                                    && orm::Criteria(models::CreditLocalizations::Cols::_language_id, orm::CompareOperator::EQ, enumToInt(language))).has_value();
}


// Возвращает response.text
std::string getImageData(const std::string& imageLink);

// Возвращает путь на файл
std::string createImageFromLink(const std::string& imageLink, const std::string& pathToDirectory);

std::optional<int64_t> CreateAndInsertImage(const std::string& imageLink, const std::string& pathToDirectory, const ImageType type, const Language language);



std::optional<int64_t> insertImage(const std::string& path, const ImageType type, const Language language);
void assignImage(const int64_t imageID, const int64_t mediaItemID);
void insertImage(const ImageType type, const Language language, const std::string& path, const int64_t mediaItemID);
void insertImage(const LibraryScanSettings& scanSettings, const ImageType type, const Language language, const std::string& imageLink, const int64_t mediaItemID);
void insertImages(const TMDBAPI::Models::PartialMediaImages& images, const Language& language, const int64_t mediaItemID);

inline bool deleteImage(const models::Images& image)
{
    std::error_code ec;
    std::string imagePath = image.getValueOfPath();
    LOG_DEBUG<<std::format("Удаляем изображение по пути {}", imagePath);
    fs::remove(imagePath, ec);
    if (ec)
        LOG_DEBUG<<std::format("Не удалось удалить изображение ошибка: {}", ec.message());
    return deleteRecordByPrimaryKey<models::Images>(image.getPrimaryKey());
}

inline bool deleteImage(const int64_t imageID)
{
    auto image = findRecordByPrimaryKeyORM<models::Images>(imageID);
    return deleteImage(*image);
}

inline bool hasImage(const int64_t mediaItemID, const ImageType type, const Language language)
{
    try
    {
        auto res = drogon::app().getDbClient()->execSqlSync("select image.id as id, image.path as path from media_item_image_assignments assignments inner join images image on (image.image_type_id = $2 and (image.language_id = $3 or image.language_id = $4)) where (assignments.media_item_id = $1) limit 1", mediaItemID, enumToInt(type), enumToInt(language), enumToInt(Language::xx));
        auto resCount = res.size();
        if (resCount > 0)
        {
            std::string path = res[0]["path"].as<std::string>();
            if (pathExists(path))
                return true;
            LOG_WARN<<std::format("Не найдено изображение на диске {}, удаляем запись", path);
            deleteRecordByPrimaryKey<models::Images>(res[0]["id"].as<int64_t>());
        }
        return false;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        return false;
    }
}

std::optional<int64_t> findTag(const std::string& name);
std::optional<int64_t> insertTag();
std::optional<int64_t> assignTag(const int64_t tagID, const int64_t mediaItemID);
std::optional<int64_t> insertTagLocalization(const int64_t tagID, const Language language, const std::string& name);
// Добавить модель KeyWords
// void insertAssignTags(const TMDBAPI::Models::Keywords, const int64_t mediaItemID);

std::optional<int64_t> findGenre(const MetaDataProvider provider, const std::string& extID, orm::DbClientPtr dbPointer = nullptr);
std::optional<int64_t> insertGenre(const MetaDataProvider provider, const std::string& extID, orm::DbClientPtr dbPointer = nullptr);

inline std::vector<models::GenreAssignments> getGenreAssignments(const int64_t mediaItemID)
{
    return findRecordsByCriteriaORM<models::GenreAssignments>(orm::Criteria(models::GenreAssignments::Cols::_media_item_id, orm::CompareOperator::EQ, mediaItemID));
}
std::vector<models::GenreAssignments> getGenreAssignments(const int64_t mediaItemID);
void copyGenres(const int64_t fromMediaItemID, const int64_t toMediaItemID);
void insertGenre(const int64_t mediaItemID, const MetaDataProvider provider, const std::string& extID, const std::string& name, const Language language, orm::DbClientPtr dbPointer = nullptr);
void insertGenreLocalization(const int64_t genreID, const Language language, const std::string& name, orm::DbClientPtr dbPointer = nullptr);
bool assignGenre(const int64_t genreID, const int64_t mediaItemID, orm::DbClientPtr dbPointer = nullptr);
bool assignGenre(models::GenreAssignments& genre);
bool isGenreAssigned(const int64_t genreID, const int64_t mediaItemID, orm::DbClientPtr dbPointer = nullptr);

inline bool isGenreLocalized(const int64_t genreID, const Language language, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    return recordExists<models::GenreLocalizations>(orm::Criteria(models::GenreLocalizations::Cols::_genre_id, orm::CompareOperator::EQ, genreID)
    && orm::Criteria(models::GenreLocalizations::Cols::_language_id, orm::CompareOperator::EQ, enumToInt(language)), dbPointer);
}

inline bool isPersonLocalized(const int64_t personID, const Language language, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    return recordExists<models::PeopleLocalizations>(orm::Criteria(models::PeopleLocalizations::Cols::_person_id, orm::CompareOperator::EQ, personID)
    && orm::Criteria(models::PeopleLocalizations::Cols::_language_id, orm::CompareOperator::EQ, enumToInt(language)), dbPointer);
}

inline std::optional<int64_t> findProductionCompany(const std::string& name, orm::DbClientPtr dbPointer)
{
    return findRecordByCriteria<models::ProductionCompanies>(orm::Criteria(models::ProductionCompanies::Cols::_name, orm::CompareOperator::EQ, name));
}
std::optional<int64_t> insertProductionCompany(const std::string& name, orm::DbClientPtr dbPointer = nullptr);
bool assignProductionCompany(const int64_t mediaItemID, const int64_t productionCompanyID, orm::DbClientPtr dbPointer = nullptr);
// Вставка/нахождение компании и привязка
std::optional<int64_t> insertProductionCompany(const int64_t mediaItemID, const std::string& name);

TMDBAPI::Languages languageToTMDBLanguage(const Language language);

Language TMDBLanguageToLanguage(const TMDBAPI::Languages language);

template<typename T>
bool isElementInContainer(const T& el, const std::vector<T>& v)
{
    if (std::find(v.begin(), v.end(), el) != v.end())
        return true;
    return false;
}

template <typename T, size_t size>
bool isElementInContainer(const T& el, const std::array<T, size>& v)
{
    if (std::find(v.begin(), v.end(), el) != v.end())
        return true;
    return false;
}