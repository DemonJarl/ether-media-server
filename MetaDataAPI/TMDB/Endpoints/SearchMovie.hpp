#pragma once 
#include "BaseAPI.hpp"
#include "Models/PartialMovieDetails.hpp"
#include "Models/PagedResult.hpp"
#include <Utils/Utils.hpp>

namespace TMDBAPI::Endpoints 
{
    // https://developer.themoviedb.org/reference/search-movie
    class SearchMovie : BaseAPI
    {
    private:
        std::string query{};
        int firstAirDateYear{0}; // Ноль значит что не передаем в запрос
        bool includeAdult{false};
        int year{0}; // Ноль значит что не передаем в запрос
        std::string region;


        SearchMovie() = delete;
    public:
        SearchMovie(const std::string& query, const bool includeAdult = false, const int firstAirDateYear = 0, const int year = 0, const std::string& region = "");
        std::string getQuery()
        {
            return query;
        }
        void setQuery(const std::string& query)
        {
            std::string cleanQuery = query;
            if (Utils::isStringEmpty(cleanQuery))
                return;
            Utils::sanitizeURL(cleanQuery);
            this->query = std::move(cleanQuery);
            // Переписать
            baseURL = BaseURL + "/" + std::to_string(version) + "/search/movie?query="+query;
        }
        int getFirstAirDateYear()
        {
            return firstAirDateYear;
        }
        void setFirstAirDateYear(const int firstAirDateYear)
        {
            if (firstAirDateYear != 0 || (firstAirDateYear < 1000 && firstAirDateYear > 9999))
                return;
            this->firstAirDateYear = firstAirDateYear;
        }
        int getYear()
        {
            return year;
        }
        void setYear(const int year)
        {
            if (year != 0 || (year < 1000 && year > 9999))
                return;
            this->year = year;
        }
        bool getIncludeAdult()
        {
            return includeAdult;
        }
        void setIncludeAdult(const bool includeAdult)
        {
            this->includeAdult = includeAdult;
        }
        std::string getRegion()
        {
            return region;
        }
        void setRegion(const std::string& region)
        {
            if (region.empty())
                return;
            this->region = region;
        }
        void clearParams()
        {
            year = 0;
            includeAdult = false;
            firstAirDateYear = 0;
            region.clear();
        }
        std::expected<Models::PagedResult<Models::PartialMovieDetails>, APIError> getSearchResult();
        std::expected<Models::PagedResult<Models::PartialMovieDetails>, APIError> getSearchResult(Languages language);
    };
}