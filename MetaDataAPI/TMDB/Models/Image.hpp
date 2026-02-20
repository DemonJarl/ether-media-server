#pragma once
#include "BaseAPI.hpp"
#include "Enums.hpp"
#include "Models/ModelDefinition.hpp"
#include <cstdint>
namespace TMDBAPI::Models 
{
    struct Image
    {
        float aspectRatio{0};
        int height{0};
        int width{0};
        std::string filePath{};
        std::string iso639_1{};
        float voteAverage{0};
        int voteCount{0};
        ModelClassDeffinitions(Image)
        

        std::string getImageLink(PosterSize size, bool secure = true)
        {
            return (((secure) ? secureImageURL : baseImageURL) + '/' + imageSizeToString(size) + filePath);
        }

        std::string getImageLink(LogoSize size, bool secure = true)
        {
            return (((secure) ? secureImageURL : baseImageURL) + '/' + imageSizeToString(size)+ filePath);
        }
        
        std::string getImageLink(BackdropSize size, bool secure = true)
        {
            return (((secure) ? secureImageURL : baseImageURL) + '/' + imageSizeToString(size) + filePath);
        }
        
        std::string getImageLink(StillSize size, bool secure = true)
        {
            return (((secure) ? secureImageURL : baseImageURL) + '/' + imageSizeToString(size) + filePath);
        }
        
        std::string getImageLink(ProfileSize size, bool secure = true)
        {
            return (((secure) ? secureImageURL : baseImageURL) + '/' + imageSizeToString(size) + filePath);
        }
    };
}