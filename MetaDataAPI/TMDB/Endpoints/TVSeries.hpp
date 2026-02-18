#pragma once
#include "BaseAPI.hpp"
#include "Models/MediaImages.hpp"
#include "Models/TVSeries.hpp"
#include <expected>
#include <Utils/Utils.hpp>
#include <string>
#include <vector>
#include "Enums/Enums.hpp"
#include "TVSeriesExternalIds.hpp"
namespace TMDBAPI::Endpoints
{
class TVSeries : public BaseAPI
{
    using model = Models::TVSeries;
private:
    int id{};
public:
    TVSeries() = delete;
    TVSeries(int id)
    {
        this->id = id;
        baseURL = BaseURL + "/" + std::to_string(version) + "/tv/" + std::to_string(id); 
    }

    int getId()
    {
        return id;
    }
    void setId(int id)
    {
        this->id = id;
        baseURL = BaseURL + "/" + std::to_string(version) + "/tv/" + std::to_string(id); 
    }

    std::expected<Models::TVSeriesExternalIds, APIError> getExternalIDs()
    {
        std::string endpoint = baseURL+"/external_ids"+Utils::createParamString(BaseAPI::language);
        return BaseAPI::getModel<Models::TVSeriesExternalIds>(endpoint);
    }

    std::expected<model, APIError> getDetails()
    {
        std::string endpoint = baseURL+Utils::createParamString(BaseAPI::language);
        return BaseAPI::getModel<model>(endpoint);
    }
    std::expected<model, APIError> getDetails(Languages language)
    {
        std::string endpoint = baseURL+Utils::createParamString(language);
        return BaseAPI::getModel<model>(endpoint);
    }

    std::expected<model, APIError> getDetails(TVAppendToResponse appendToResponse, const std::vector<Languages>& imageLanguages = {})
    {
        std::string endpoint = baseURL+Utils::createParamString(BaseAPI::language, Utils::appendToResponseToStr(appendToResponse));
        return BaseAPI::getModel<model>(endpoint);
    }

    std::expected<model, APIError> getDetails(Languages language, TVAppendToResponse appendToResponse, const std::vector<Languages>& imageLanguages = {})
    {
        std::string endpoint = baseURL+Utils::createParamString(language, Utils::appendToResponseToStr(appendToResponse));
        return BaseAPI::getModel<model>(endpoint);
    }

    std::expected<Models::MediaImages, APIError> getImages(const std::vector<Languages>& imageLanguages = {})
    {
        std::string endpoint = baseURL+"/images"+Utils::createParamString(BaseAPI::language);
        if (!imageLanguages.empty())
        {
            endpoint+="&image_languages="+Utils::imageLanguagesToStr(imageLanguages);
        }
        return BaseAPI::getModel<Models::MediaImages>(endpoint);
    }
};
}