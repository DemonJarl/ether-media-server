#pragma once
#include "BaseAPI.hpp"
#include "Enums/Enums.hpp"
#include "Models/PagedResult.hpp"
#include "Models/PartialTVDetails.hpp"
#include "Utils/Utils.hpp"
#include <expected>
#include <string>
#include <boost/algorithm/string.hpp>

namespace TMDBAPI::Endpoints 
 {
    class SearchTV : BaseAPI
    {
    private:
        std::string query{};
        int firstAirDateYear{0}; // Ноль значит что не передаем в запрос
        bool includeAdult{false};
        int year{0}; // Ноль значит что не передаем в запрос

        SearchTV() = delete;
    public:
        SearchTV(const std::string& query, bool includeAdult = false, int firstAirDateYear = 0, int year = 0);
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
            baseURL = BaseURL + "/" + std::to_string(version) + "/search/tv?query="+query;
        }
        int getFirstAirDateYear()
        {
            return firstAirDateYear;
        }
        void setFirstAirDateYear(const int firstAirDateYear)
        {
            if (firstAirDateYear < 1000 || firstAirDateYear > 9999)
                return;
            this->firstAirDateYear = firstAirDateYear;
        }
        int getYear()
        {
            return year;
        }
        void setYear(const int year)
        {
            if (year < 1000 || year > 9999)
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
        void clearParams()
        {
            year = 0;
            includeAdult = false;
            firstAirDateYear = 0;
        }
        std::expected<Models::PagedResult<Models::PartialTVDetails>, APIError> getSearchResult();
        std::expected<Models::PagedResult<Models::PartialTVDetails>, APIError> getSearchResult(Languages language);
    };
 }