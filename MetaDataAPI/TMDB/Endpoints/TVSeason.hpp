#pragma once
#include "BaseAPI.hpp"
#include "Enums.hpp"
#include "Models/MediaImages.hpp"
#include "Models/TVSeason.hpp"
#include "TVSeasonExternalIds.hpp"
#include <Utils/Utils.hpp>
#include <string>
namespace TMDBAPI::Endpoints 
{
    struct TVSeason : BaseAPI
    {
        int seriesId{};
        int seasonId{};

        TVSeason(const int seriesId, const int seasonId)
        {
            this->seriesId = seriesId;
            this->seasonId = seasonId;
            baseURL = BaseURL + "/" + std::to_string(version) + "/tv/" + std::to_string(seriesId) + "/season/" + std::to_string(seasonId); 
            
        }
        TVSeason() : TVSeason(0, 0){};

        int getSeriesId()
        {
            return seriesId;
        }

        int getSeasonId()
        {
            return seasonId;
        }

        void setSeriesId(const int id)
        {
            if (seriesId != id)
            {
                boost::replace_first(baseURL, std::to_string(seriesId), std::to_string(id));
                seriesId = id;
            }
        }

        void setSeasonId(const int id)
        {
            if (seasonId != id)
            {
                boost::replace_last(baseURL, std::to_string(seasonId), std::to_string(id));
                seriesId = id;
            }
        }

        std::expected<Models::TVSeasonExternalIds, APIError> getExternalIDs()
        {
            std::string endpoint = baseURL+"/external_ids"+Utils::createParamString(BaseAPI::language);
            return BaseAPI::getModel<Models::TVSeasonExternalIds>(endpoint);
        }

        std::expected<Models::TVSeason, APIError> getDetails()
        {
            std::string endpoint = baseURL+Utils::createParamString(BaseAPI::language);
            return BaseAPI::getModel<Models::TVSeason>(endpoint);
        }

        std::expected<Models::TVSeason, APIError> getDetails(const Languages language)
        {
            std::string endpoint = baseURL+Utils::createParamString(language);
            return BaseAPI::getModel<Models::TVSeason>(endpoint);
        }

        std::expected<Models::TVSeason, APIError> getDetails(TVSeasonAppendToResponse appendToResponse, std::vector<Languages> imageLanguages)
        {
            std::string endpoint = baseURL+Utils::createParamString(language, Utils::appendToResponseToStr(appendToResponse));
            if (!imageLanguages.empty())
            {
                endpoint+="&image_languages="+Utils::imageLanguagesToStr(imageLanguages);
            }
            return BaseAPI::getModel<Models::TVSeason>(endpoint);
        }
        
        std::expected<Models::TVSeason, APIError> getDetails(Languages language, TVSeasonAppendToResponse appendToResponse, std::vector<Languages> imageLanguages = {})
        {
            std::string endpoint = baseURL+Utils::createParamString(language, Utils::appendToResponseToStr(appendToResponse));
            if (!imageLanguages.empty())
            {
                endpoint+="&image_languages="+Utils::imageLanguagesToStr(imageLanguages);
            }
            return BaseAPI::getModel<Models::TVSeason>(endpoint);
        }

        std::expected<Models::MediaImages, APIError> getImages(std::vector<Languages> imageLanguages)
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