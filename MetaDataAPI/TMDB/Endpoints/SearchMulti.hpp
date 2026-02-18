#pragma once
#include "BaseAPI.hpp"
#include "Models/SearchPersonDetails.hpp"
#include "Models/PagedResult.hpp"
#include "Models/PartialMovieDetails.hpp"
#include "Models/PartialTVDetails.hpp"
#include "Utils/Utils.hpp"
#include <variant>

namespace TMDBAPI::Endpoints 
{
    class SearchMulti : BaseAPI
    {
        using Multi = std::variant<Models::SearchPersonDetails, Models::PartialTVDetails, Models::PartialMovieDetails>;
        std::string query{};
        bool includeAdult{false};

        SearchMulti() = delete;
    public:
        SearchMulti(const std::string& query, bool includeAdult = false)
        {
            std::string cleanQuery = query;
            if (Utils::isStringEmpty(cleanQuery))
                throw std::invalid_argument("Поисковый запрос не может быть пустой");
            Utils::sanitizeURL(cleanQuery);
            this->query = std::move(cleanQuery);
            baseURL = BaseURL + "/" + std::to_string(version) + "/search/multi?query="+this->query;

            this->includeAdult = includeAdult;
        }
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
            baseURL = BaseURL + "/" + std::to_string(version) + "/search/multi?query="+query;
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
            includeAdult = false;
        }
        // std::expected<Models::PagedResult<Multi>, APIError> getSearchResult()
        // {
        //     std::string endpoint = baseURL + Utils::createParamString(BaseAPI::language, false)+"&include_adult="+Utils::boolToString(includeAdult);
        //     return getPagedResult<Models::PagedResult<Multi>>(endpoint);
        // }
        // std::expected<Models::PagedResult<Multi>, APIError> getSearchResult(Languages language)
        // {
        //     std::string endpoint = baseURL + Utils::createParamString(language, false)+"&include_adult="+Utils::boolToString(includeAdult);
        //     return getPagedResult<Models::PagedResult<Multi>>(endpoint);
        // }
    };
}