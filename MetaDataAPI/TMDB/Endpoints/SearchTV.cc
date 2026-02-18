#include "Endpoints/SearchTV.hpp"
#include "BaseAPI.hpp"
#include "Enums/Enums.hpp"
#include <Utils/Utils.hpp>
#include "Models/PagedResult.hpp"
#include "Models/PartialTVDetails.hpp"
#include <stdexcept>
#include <string>
namespace TMDBAPI::Endpoints 
{
    SearchTV::SearchTV(const std::string& query, bool includeAdult, int firstAirDateYear, int year)
    {
        std::string cleanQuery = query;
        if (Utils::isStringEmpty(cleanQuery))
            throw std::invalid_argument("Поисковый запрос не может быть пустой");
        Utils::sanitizeURL(cleanQuery);
        this->query = std::move(cleanQuery);
        baseURL = BaseURL + "/" + std::to_string(version) + "/search/tv?query="+this->query;

        this->includeAdult = includeAdult;

        if (firstAirDateYear != 0 || (firstAirDateYear < 1000 && firstAirDateYear > 9999))
            throw std::invalid_argument("Год первого эфира должен быть между 1000 и 9999");
        this->firstAirDateYear = firstAirDateYear;

        if (year != 0 || (year < 1000 && year > 9999))
            throw std::invalid_argument("Год должен быть между 1000 и 9999");
        this->year = year;
    }
    
    std::expected<Models::PagedResult<Models::PartialTVDetails>, APIError> SearchTV::getSearchResult()
    {
        std::string endpoint = baseURL + Utils::createParamString(BaseAPI::language, false)+"&include_adult="+Utils::boolToString(includeAdult);
        if (firstAirDateYear != 0)
            endpoint += "&first_air_date_year="+std::to_string(firstAirDateYear);
        if (year != 0)
            endpoint += "&year="+std::to_string(year);
        return getPagedResult<Models::PagedResult<Models::PartialTVDetails>>(endpoint);
    }
    std::expected<Models::PagedResult<Models::PartialTVDetails>, APIError> SearchTV::getSearchResult(Languages language)
    {
        std::string endpoint = baseURL + Utils::createParamString(language, false)+"&include_adult="+Utils::boolToString(includeAdult);
        if (firstAirDateYear != 0)
            endpoint += "&first_air_date_year="+std::to_string(firstAirDateYear);
        if (year != 0)
            endpoint += "&year="+std::to_string(year);
        return getPagedResult<Models::PagedResult<Models::PartialTVDetails>>(endpoint);
    }
}