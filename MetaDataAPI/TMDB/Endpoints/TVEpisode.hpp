#pragma once
#include "BaseAPI.hpp"
#include "Models/MediaImages.hpp"
#include "Models/TVEpisode.hpp"
#include <expected>
#include <Utils/Utils.hpp>
#include <string>
#include <vector>
#include "Enums/Enums.hpp"
#include "TVEpisode.hpp"
#include "TVEpisodeExternalIds.hpp"
#include "TVSeriesExternalIds.hpp"
namespace TMDBAPI::Endpoints
{
class TVEpisode : public BaseAPI
{
    using model = Models::TVEpisode;
private:
    int showID{};
    int season{};
    int episode{};
public:
    TVEpisode() = delete;
    TVEpisode(int showID, int season, int episode)
    {
        this->showID = showID;
        this->season = season;
        this->episode = episode;
        baseURL = BaseURL + "/" + std::to_string(version) + "/tv/" + std::to_string(showID) + "/season/" + std::to_string(season) + "/episode/" + std::to_string(episode); 
    }
    
    std::expected<Models::TVEpisodeExternalIds, APIError> getExternalIDs()
    {
        std::string endpoint = baseURL+"/external_ids"+Utils::createParamString(BaseAPI::language);
        return BaseAPI::getModel<Models::TVEpisodeExternalIds>(endpoint);
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
        if (!imageLanguages.empty())
        {
            endpoint+="&image_languages="+Utils::imageLanguagesToStr(imageLanguages);
        }
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