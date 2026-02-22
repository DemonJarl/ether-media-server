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
#include "CreditAssignments.h"
#include "CreditLocalizations.h"
#include "Credits.h"
#include "CrewCredit.hpp"
#include "DBUtils.h"
#include "EnumToInt.h"
#include "Enums.hpp"
#include "ExternalMediaItemIds.h"
#include "ExternalPeopleIds.h"
#include "GenreAssignments.h"
#include "GenreLocalizations.h"
#include "Genres.h"
#include "Images.h"
#include "MediaEnums.h"
#include "MediaItemLocalizations.h"
#include "MediaItemStreams.h"
#include "MediaItems.h"
#include "MovieExternalIds.hpp"
#include "People.h"
#include "PeopleLocalizations.h"
#include "MetaDataAPI/TMDB/Models/TVSeriesAggregateCredits.hpp"
#include "PersonExternalIds.hpp"
#include "ProductionCompanies.h"
#include "LibrarySettingsManager.hpp"
#include "TVEpisodeExternalIds.hpp"
#include "TVSeasonExternalIds.hpp"
#include "TVSeriesExternalIds.hpp"
#include "TimeUtils.h"
#include "Utils.h"
#include "coro/sync_wait.hpp"
#include "coro/task.hpp"
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
    bool collectEpisodeCredits = true;
    bool collectSeasonCredits = true;
    bool collectMovieCredits = true;
    bool collectShowCredits = true;
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

coro::task<std::optional<int64_t>> insertPath(const int64_t libraryID, const std::string& path);
coro::task<bool> deletePath(const int64_t pathID);
coro::task<std::vector<int64_t>> getMediaItemIDsByPath(const int64_t pathID);
coro::task<std::vector<models::MediaItems>> getMediaItemsByPath(const int64_t pathID);
coro::task<bool> deleteMediaItem(const models::MediaItems& mediaItem);
coro::task<bool> deleteMediaItem(const int64_t mediaItemID);
coro::task<std::vector<int64_t>> getImageIDsForMediaItem(const int64_t mediaItemID);
coro::task<std::vector<int64_t>> getImageIDsForMediaItem(const int64_t mediaItemID, const std::vector<Language>& languages);
coro::task<std::vector<models::Images>> getImagesForMediaItem(const int64_t mediaItemID);
coro::task<std::vector<models::Images>> getImagesForMediaItem(const int64_t mediaItemID, const std::vector<Language>& languages);

void scanLibrary(const int64_t, ScanMode mode, orm::DbClientPtr dbPointer = nullptr);
void scanLibrary(const models::Libraries& library, ScanMode mode, orm::DbClientPtr dbPointer = nullptr);
void scanLibraries(ScanMode mode, orm::DbClientPtr dbPointer = nullptr);
void scanDirectory(fs::directory_entry directory);
coro::task<void> analyzeFile(const fs::directory_entry& file, const LibraryScanSettings& scanSettings);
void scanPath(const fs::path& path, const std::vector<std::string>& scanedPaths, const LibraryScanSettings& scanSettings);
std::vector<models::MediaItemStreams> createStreams(AVFormatContext* ctx);
coro::task<std::optional<int64_t>> insertMediaItemLocalization(const int64_t  mediaItemID, const Language language, const std::string& name, const std::string& description, const std::string& tagline, const bool original = false);
coro::task<std::optional<models::MediaItemLocalizations>> findMediaItemLocalization(const int64_t mediaItemID, const Language language, orm::DbClientPtr dbPointer = nullptr);
std::vector<fs::directory_entry> enumerateFiles(const std::string& path);
std::vector<fs::directory_entry> enumerateFiles(std::vector<std::string> path);

coro::task<void> assignMediaItemToLibraryByPath(const int64_t mediaItemID, const std::string& path);
coro::task<void> assignMediaItemToLibraryByID(const int64_t mediaItemID, const int64_t libraryID);


void insertStreams(AVStream** streams, uint streamCount, int64_t mediaItemID, orm::DbClientPtr dbPointer = nullptr);
models::MediaItemStreams parseStream(AVStream& stream, int64_t mediaItemID);
void parseFileName(const std::string& fileName, std::string& name, int& season, int& episode, int& year);

bool getDataFromMetadataProviders(const LibraryScanSettings& scanSettings, const models::MediaItems& mediaItem);

coro::task<std::vector<models::ExternalMediaItemIds>> getMediaItemExternalID(const models::MediaItems& mediaItem, orm::DbClientPtr dbPointer = nullptr);
coro::task<std::vector<models::ExternalMediaItemIds>> getMediaItemExternalID(const int64_t mediaItemID, orm::DbClientPtr dbPointer = nullptr);
coro::task<std::vector<models::ExternalMediaItemIds>> getShowExternalIDFromSeason(const int64_t seasonMediaItemID, orm::DbClientPtr dbPointer = nullptr);

coro::task<bool> getTVShowMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);
coro::task<bool> getTVSeasonMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem, const std::vector<models::ExternalMediaItemIds>& showExternalIDs);
coro::task<bool> getTVEpisodeMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);
coro::task<bool> getMovieMetaData(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);

coro::task<bool> getTVSeasonMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);
coro::task<bool> getTVEpisodeMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem, const int TMDBShowID);
coro::task<bool> getMovieMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);
coro::task<bool> getTVShowMetaDataTMDB(const LibraryScanSettings& scanSettings, models::MediaItems& mediaItem);
coro::task<bool> getMovieMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBid, const int64_t mediaItemID, const Language language, const bool original = false);
coro::task<bool> getTVSeasonMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBShowID, const int TMDBSeasonID, const int64_t mediaItemID, const Language language, const bool original = false);
coro::task<bool> getTVShowMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBid, const int64_t mediaItemID, const Language language, const bool original = false);
coro::task<bool> getTVEpisodeMetaDataTMDB(const LibraryScanSettings& scanSettings, const int TMDBShowID, const int season, const int episode, const int64_t mediaItemID, const Language language, const bool original = false);

coro::task<void> updateExtIds(const TMDBAPI::Models::PersonExternalIds& extIDs, const int64_t personID);
coro::task<void> updateExtIds(const TMDBAPI::Models::MovieExternalIds& extIDs, const int64_t mediaItemID);
coro::task<void> updateExtIds(const TMDBAPI::Models::TVEpisodeExternalIds& extIDs, const int64_t mediaItemID);
coro::task<void> updateExtIds(const TMDBAPI::Models::TVSeriesExternalIds& extIDs, const int64_t mediaItemID);
coro::task<void> updateExtIds(const TMDBAPI::Models::TVSeasonExternalIds& extIDs, const int64_t mediaItemID);
coro::task<void> updateExtIds(const TMDBAPI::Models::PersonExternalIds& extIDs, const int64_t personID);

inline Language getOriginalLanguage(const int64_t mediaItemID)
{
    auto loc = coro::sync_wait(findRecordByCriteriaORM<models::MediaItemLocalizations>(orm::Criteria(models::MediaItemLocalizations::Cols::_media_item_id, orm::CompareOperator::EQ, mediaItemID)
                                                                        && orm::Criteria(models::MediaItemLocalizations::Cols::_original, orm::CompareOperator::EQ, true)));
    if (!loc.has_value())
        return Language::xx;
    return static_cast<Language>(loc->getValueOfLanguageId());
}

coro::task<std::optional<int64_t>> getParentForEpisode(const std::string& showTitle, const int season, const std::string& releaseDate, const LibraryScanSettings& scanSettings, orm::DbClientPtr dbPointer = nullptr);

coro::task<std::optional<models::MediaItems>> createMediaItem(const LibraryScanSettings& scanSettings, MediaType type, int64_t parentID, const std::string& parsedTitle, const std::string& path, const int season, const int episode, const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);
coro::task<std::optional<int64_t>> createTVShowMediaItem(const LibraryScanSettings& scanSettings, const std::string& parsedTitle, const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);
coro::task<std::optional<int64_t>> createTVSeasonMediaItem(const LibraryScanSettings& scanSettings, const int64_t parentID, const int season, const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);
coro::task<std::optional<int64_t>> createTVEpisodeMediaItem(const LibraryScanSettings& scanSettings, const int64_t parentID, const std::string& parsedTitle, const std::string& path, const int season, const int episode,  const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);




inline coro::task<void> updateMediaItemTimestamp(const int64_t mediaItemId)
{
    //auto mediaItem = findRecordByPrimaryKeyORM<models::MediaItems>(mediaItemId);
    if (!(co_await execSQL(nullptr, "update media_items set last_updated = $1 where id = $2", getCurrentDateTime(), mediaItemId)).has_value())
        LOG_ERROR<<"Не удалось обновить время последнего обновления медиа предмета"; 
    co_return;
}   

inline coro::task<void> updateMediaItemTimestamp(models::MediaItems& mediaItem, orm::DbClientPtr dbPointer)
{
    if(!dbPointer)
        dbPointer = app().getDbClient();
    mediaItem.setLastUpdated(getCurrentDateTime());
    if (!co_await updateRecord(mediaItem, dbPointer))
        LOG_ERROR<<"Не удалось обновить время последнего обновления медиа предмета"; 
    co_return;
}

inline coro::task<void> updateMediaItemTimestamp(models::MediaItems& mediaItem, orm::Mapper<models::MediaItems>& mp)
{
    mediaItem.setLastUpdated(getCurrentDateTime());
    if (!co_await updateRecord(mediaItem, mp))
        LOG_ERROR<<"Не удалось обновить время последнего обновления медиа предмета"; 
    co_return;
}

coro::task<std::optional<int64_t>> findSeasonMediaItemID(const std::string& seriesTitle, const int64_t season, const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);
coro::task<std::optional<int64_t>> findShowMediaItemID(const std::string& seriesTitle, const std::string& releaseDate, orm::DbClientPtr dbPointer = nullptr);

coro::task<std::optional<int64_t>> findShowByExternalID(const std::vector<std::pair<std::string, MetaDataProvider>> exteranlIDs);
coro::task<std::optional<int64_t>> findSeasonByExternalID(const std::vector<std::pair<std::string, MetaDataProvider>> exteranlIDs);
coro::task<std::optional<int64_t>> insertMediaItemExternalID(const int64_t mediaItemID, const MetaDataProvider provider, const std::string& externalID);
coro::task<std::optional<int64_t>> insertPerson(const MetaDataProvider provider, const std::string& id, const Language language);
coro::task<std::optional<int64_t>> insertPersonTMDB(const int id, const Language language);
coro::task<std::optional<int64_t>> findOrInsertPerson(const MetaDataProvider provider, const std::string& id, const Language language, const std::string& name);
coro::task<void> attachImageToPerson(const int64_t personID, const MetaDataProvider provider, const std::string& id);

std::vector<models::MediaItems> findMediaItemsByPerson(const int64_t personID, const CreditType creditType);
std::vector<models::MediaItems> findMediaItemsByProductionCompany(const int64_t productionCompanyID);
std::vector<models::MediaItems> findMediaItemsByGenre(const std::vector<int64_t>& genreID);
inline coro::task<std::optional<models::MediaItems>> findMediaItemByPath(const std::string& path)
{
    co_return (co_await findRecordByCriteriaORM<models::MediaItems>(orm::Criteria(models::MediaItems::Cols::_path, orm::CompareOperator::EQ, path)));
}


std::optional<models::People> findPersonORM(const std::string& name, const std::string& dateOfBirth, orm::DbClientPtr dbPointer = nullptr);
std::optional<models::People> findPersonORM(const models::ExternalPeopleIds& extID, orm::DbClientPtr dbPointer = nullptr);
std::optional<models::People> findPersonORM(const MetaDataProvider provider, const std::string& externalID, orm::DbClientPtr dbPointer = nullptr);
std::optional<models::People> findPersonORM(const std::vector<models::ExternalPeopleIds>& extIDs, orm::DbClientPtr dbPointer = nullptr);

coro::task<std::optional<int64_t>> findPerson(const std::string& name, const std::string& dateOfBirth, orm::DbClientPtr dbPointer = nullptr);
coro::task<std::optional<int64_t>> findPerson(const std::string& name, const Gender gender, orm::DbClientPtr dbPointer = nullptr);
coro::task<std::optional<int64_t>> findPerson(const models::ExternalPeopleIds& extID, orm::DbClientPtr dbPointer = nullptr);
coro::task<std::optional<int64_t>> findPerson(const MetaDataProvider provider, const std::string& externalID, orm::DbClientPtr dbPointer = nullptr);
coro::task<std::optional<int64_t>> findPerson(const std::vector<models::ExternalPeopleIds>& extIDs, orm::DbClientPtr dbPointer = nullptr);

coro::task<std::optional<int64_t>> findPeople(const std::string& name, orm::DbClientPtr dbPointer = nullptr);
std::vector<int64_t> findPeople(const CreditType type);
std::vector<int64_t> findPeople(const int64_t mediaItemID, const CreditType type);
std::vector<int64_t> findPeople(const int64_t mediaItemID);
std::vector<models::People> findPeopleORM(const CreditType type);
std::vector<models::People> findPeopleORM(const int64_t mediaItemID, const CreditType type);
std::vector<models::People> findPeopleORM(const int64_t mediaItemID);

std::optional<models::People> insertPersonORM(const std::string& dateOfBirth, orm::DbClientPtr dbPointer = nullptr);
std::optional<models::PeopleLocalizations> insertPersonLocalizationORM(int64_t personID, const std::string& name, const std::string& biography, const Language language, orm::DbClientPtr dbPointer = nullptr);
std::optional<models::ExternalPeopleIds> insertPersonExternalIDORM(const int64_t personID, const MetaDataProvider provider, const std::string& externalID);
 
// Проходим по списку языков необходимых для локализации и смотрим есть ли локализация, нету вставляем
coro::task<void> localizePersonTMDB(const int64_t personID, const int id, const Language language);
coro::task<std::optional<int64_t>> insertPersonLocalization(int64_t personID, const std::string& name, const std::string& biography, const Language language);
coro::task<std::optional<models::PeopleLocalizations>> findPersonLocalizationORM(const int64_t personID, const Language language);
//bool personHasLocalization(const int64_t personID, const Language language);
coro::task<std::optional<int64_t>> insertPersonExternalID(const int64_t personID, const MetaDataProvider provider, const std::string& externalID);

coro::task<std::optional<int64_t>> insertCreditLocalization(const int64_t creditID, const Language language, const std::string& text);

// копируем credit и локализацию с привязкой на ToMediaItemID
void copyCredit(const int64_t creditID, const int64_t ToMediaItemID);

coro::task<std::optional<int64_t>> insertCredit(const TMDBAPI::Models::TVSeriesAggregateCredits& credit, const Language language, const int64_t mediaItemID);


coro::task<std::optional<int64_t>> insertCredit(const int64_t personID, const CreditType creditType, const MetaDataProvider origin, const std::string& externalID);
coro::task<std::optional<int64_t>> insertCredit(const int64_t personID, const int64_t mediaItemID, const CreditType creditType, const MetaDataProvider origin, const std::string& externalID);
coro::task<std::optional<int64_t>> findOrInsertCredit(const int64_t mediaItemID, const int64_t personID, const CreditType type, const MetaDataProvider origin, const std::string& externalID);

coro::task<void> insertCredits(const std::vector<TMDBAPI::Models::CastCredit>&, const Language language, const int64_t mediaItemID);
coro::task<void> insertCredits(const std::vector<TMDBAPI::Models::CrewCredit>&, const Language language, const int64_t mediaItemID);

std::vector<models::Credits> findCredits(const int64_t personID);
std::vector<models::Credits> findCredits(const int64_t personID, const CreditType creditType);
std::vector<int64_t> findCredits(const std::string& role, const CreditType creditType);
std::vector<int64_t> findCredits(const std::string& role, const CreditType creditType);
coro::task<std::optional<int64_t>> findCredit(const int64_t personID, const int64_t mediaItemID, const CreditType creditType, const std::string& role);
coro::task<std::optional<int64_t>> findCredits(const int64_t personID, const int64_t mediaItemID, const CreditType creditType);
inline coro::task<std::optional<int64_t>> findCredit(const MetaDataProvider origin, const std::string& externalID)
{
    co_return co_await findRecordByCriteria<models::Credits>(orm::Criteria(models::Credits::Cols::_origin, orm::CompareOperator::EQ, enumToInt(origin))
                                                    && orm::Criteria(models::Credits::Cols::_external_id, orm::CompareOperator::EQ, externalID));
}
inline coro::task<bool> isCreditLocalized(const int64_t creditID, const Language language)
{
    co_return (co_await findRecordByCriteria<models::CreditLocalizations>(orm::Criteria(models::CreditLocalizations::Cols::_credit_id, orm::CompareOperator::EQ, creditID)
                                                    && orm::Criteria(models::CreditLocalizations::Cols::_language_id, orm::CompareOperator::EQ, enumToInt(language)))).has_value();
}

inline coro::task<void> assignCredit(const int64_t creditID, const int64_t mediaItemID)
{
    models::CreditAssignments assignment;
    assignment.setCreditId(creditID);
    assignment.setMediaItemId(mediaItemID);
    co_await insertRecord(assignment);
}

// Возвращает response.text
coro::task<std::string> getImageData(const std::string& imageLink);

// Возвращает путь на файл
coro::task<std::string>  createImageFromLink(const std::string& imageLink, const std::string& pathToDirectory);

coro::task<std::optional<int64_t>> CreateAndInsertImage(const MetaDataProvider origin, const std::string& imageLink, const std::string& pathToDirectory, const ImageType type, const Language language);

coro::task<std::optional<int64_t>> insertImage(const std::string& path, const ImageType type, const Language language, const MetaDataProvider origin, const std::string& imageLink);
coro::task<void> assignImage(const int64_t imageID, const int64_t mediaItemID);
coro::task<void> insertImage(const ImageType type, const Language language, const std::string& path, const MetaDataProvider origin, const std::string& imageLink, const int64_t mediaItemID);
coro::task<void> insertImage(const MetaDataProvider origin, const ImageType type, const Language language, const std::string& imageLink, const int64_t mediaItemID);

inline coro::task<std::optional<models::Images>> findImageORM(const MetaDataProvider origin, const std::string& link)
{
    co_return (co_await findRecordByCriteriaORM<models::Images>(orm::Criteria(models::Images::Cols::_origin, orm::CompareOperator::EQ, enumToInt(origin))
    && orm::Criteria(models::Images::Cols::_image_link, orm::CompareOperator::EQ, link)));
}

inline coro::task<bool> deleteImage(const models::Images& image)
{
    std::string imagePath = image.getValueOfPath();
    LOG_DEBUG<<std::format("Удаляем изображение по пути {}", imagePath);
    if (!deleteFile(imagePath))
        LOG_DEBUG<<"Не удалось удалить изображение";
    co_return co_await deleteRecordByPrimaryKey<models::Images>(image.getPrimaryKey());
}

inline coro::task<bool> deleteImage(const int64_t imageID)
{
    auto image = coro::sync_wait(findRecordByPrimaryKeyORM<models::Images>(imageID));
    co_return co_await deleteImage(*image);
}

inline coro::task<bool> hasImage(const int64_t mediaItemID, const ImageType type, const Language language)
{
    try
    {
        auto res = co_await drogon::app().getDbClient()->execSqlCoro("select image.id as id, image.path as path from media_item_image_assignments assignments inner join images image on (image.image_type_id = $2 and (image.language_id = $3 or image.language_id = $4)) where (assignments.media_item_id = $1) limit 1", mediaItemID, enumToInt(type), enumToInt(language), enumToInt(Language::xx));
        auto resCount = res.size();
        if (resCount > 0)
        {
            std::string path = res[0]["path"].as<std::string>();
            if (pathExists(path))
                co_return true;
            LOG_WARN<<std::format("Не найдено изображение на диске {}, удаляем запись", path);
            co_await deleteRecordByPrimaryKey<models::Images>(res[0]["id"].as<int64_t>());
        }
        co_return false;
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR<<"Ошибка запроса: "<<e.base().what();
        co_return false;
    }
}

coro::task<std::optional<int64_t>> findTag(const std::string& name);
coro::task<std::optional<int64_t>> insertTag();
coro::task<std::optional<int64_t>> assignTag(const int64_t tagID, const int64_t mediaItemID);
coro::task<std::optional<int64_t>> insertTagLocalization(const int64_t tagID, const Language language, const std::string& name);
// Добавить модель KeyWords
// void insertAssignTags(const TMDBAPI::Models::Keywords, const int64_t mediaItemID);

inline coro::task<std::optional<int64_t>> findGenre(const MetaDataProvider provider, const std::string& extID, orm::DbClientPtr dbPointer = nullptr)
{
    co_return co_await findRecordByCriteria<models::Genres>(orm::Criteria(models::Genres::Cols::_origin, orm::CompareOperator::EQ, enumToInt(provider))
                                                && orm::Criteria(models::Genres::Cols::_external_id, orm::CompareOperator::EQ, extID), dbPointer);
}
coro::task<std::optional<int64_t>> insertGenre(const MetaDataProvider provider, const std::string& extID, orm::DbClientPtr dbPointer = nullptr);

inline coro::task<std::vector<models::GenreAssignments>> getGenreAssignments(const int64_t mediaItemID)
{
    co_return co_await findRecordsByCriteriaORM<models::GenreAssignments>(orm::Criteria(models::GenreAssignments::Cols::_media_item_id, orm::CompareOperator::EQ, mediaItemID));
}
void copyGenres(const int64_t fromMediaItemID, const int64_t toMediaItemID);
coro::task<void> insertGenre(const int64_t mediaItemID, const MetaDataProvider provider, const std::string& extID, const std::string& name, const Language language, orm::DbClientPtr dbPointer = nullptr);
coro::task<void> insertGenreLocalization(const int64_t genreID, const Language language, const std::string& name, orm::DbClientPtr dbPointer = nullptr);
coro::task<bool> assignGenre(const int64_t genreID, const int64_t mediaItemID, orm::DbClientPtr dbPointer = nullptr);
coro::task<bool> assignGenre(models::GenreAssignments& genre);
coro::task<bool> isGenreAssigned(const int64_t genreID, const int64_t mediaItemID, orm::DbClientPtr dbPointer = nullptr);

inline coro::task<bool> isGenreLocalized(const int64_t genreID, const Language language, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    co_return co_await recordExists<models::GenreLocalizations>(orm::Criteria(models::GenreLocalizations::Cols::_genre_id, orm::CompareOperator::EQ, genreID)
    && orm::Criteria(models::GenreLocalizations::Cols::_language_id, orm::CompareOperator::EQ, enumToInt(language)), dbPointer);
}

inline coro::task<bool> isPersonLocalized(const int64_t personID, const Language language, orm::DbClientPtr dbPointer = nullptr)
{
    if (!dbPointer)
        dbPointer = app().getDbClient();
    co_return co_await recordExists<models::PeopleLocalizations>(orm::Criteria(models::PeopleLocalizations::Cols::_person_id, orm::CompareOperator::EQ, personID)
    && orm::Criteria(models::PeopleLocalizations::Cols::_language_id, orm::CompareOperator::EQ, enumToInt(language)), dbPointer);
}

inline coro::task<std::optional<int64_t>> findProductionCompany(const std::string& name, orm::DbClientPtr dbPointer)
{
    co_return co_await findRecordByCriteria<models::ProductionCompanies>(orm::Criteria(models::ProductionCompanies::Cols::_name, orm::CompareOperator::EQ, name), dbPointer);
}
coro::task<std::optional<int64_t>> insertProductionCompany(const std::string& name, orm::DbClientPtr dbPointer = nullptr);
coro::task<bool> assignProductionCompany(const int64_t mediaItemID, const int64_t productionCompanyID, orm::DbClientPtr dbPointer = nullptr);
// Вставка/нахождение компании и привязка
coro::task<std::optional<int64_t>> insertProductionCompany(const int64_t mediaItemID, const std::string& name);

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