#include "SearchMovie.hpp"
namespace TMDBAPI::Endpoints
{
    SearchMovie::SearchMovie(const std::string& query, const bool includeAdult, const int firstAirDateYear, const int year, const std::string& region)
    {
        std::string cleanQuery = query;
        if (Utils::isStringEmpty(cleanQuery))
            throw std::invalid_argument("Поисковый запрос не может быть пустой");
        Utils::sanitizeURL(cleanQuery);
        this->query = std::move(cleanQuery);
        baseURL = BaseURL + "/" + std::to_string(version) + "/search/movie?query="+this->query;

        this->includeAdult = includeAdult;

        if (firstAirDateYear != 0 && (firstAirDateYear < 1000 || firstAirDateYear > 9999))
            throw std::invalid_argument("Год первого эфира должен быть между 1000 и 9999");
        this->firstAirDateYear = firstAirDateYear;

        if (year != 0 && (year < 1000 || year > 9999))
            throw std::invalid_argument("Год должен быть между 1000 и 9999");
        this->year = year;
        
        if (!region.empty())
            this->region = region;
    }
    
    std::expected<Models::PagedResult<Models::PartialMovieDetails>, APIError> SearchMovie::getSearchResult()
    {
        std::string endpoint = baseURL + Utils::createParamString(BaseAPI::language, false)+"&include_adult="+Utils::boolToString(includeAdult);
        if (firstAirDateYear != 0)
            endpoint += "&first_air_date_year="+std::to_string(firstAirDateYear);
        if (year != 0)
            endpoint += "&year="+std::to_string(year);
        return getPagedResult<Models::PagedResult<Models::PartialMovieDetails>>(endpoint);
    }
    std::expected<Models::PagedResult<Models::PartialMovieDetails>, APIError> SearchMovie::getSearchResult(Languages language)
    {
        std::string endpoint = baseURL + Utils::createParamString(language, false)+"&include_adult="+Utils::boolToString(includeAdult);
        if (firstAirDateYear != 0)
            endpoint += "&first_air_date_year="+std::to_string(firstAirDateYear);
        if (year != 0)
            endpoint += "&year="+std::to_string(year);
        return getPagedResult<Models::PagedResult<Models::PartialMovieDetails>>(endpoint);
    }
}