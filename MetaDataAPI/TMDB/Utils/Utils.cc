#include "Utils.hpp"
#include "Enums/Enums.hpp"
#include "RequestParams/ListRequestParam.hpp"
#include <iterator>
#include <memory>
#include <string>
namespace TMDBAPI::Utils
{

    #define MATCH_ENUM(_enumVal, _match)\
        if((_enumVal & _match) == _match)\

    std::string appendToResponseToStr(TVAppendToResponse appendToResponse)
    {
        std::string ret{};
        if (appendToResponse == TVAppendToResponse::None)
            return ret;
        MATCH_ENUM(appendToResponse, TVAppendToResponse::AccountStates)
            ret += (ret.empty()) ? "account_states" : ",account_states";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::AggregateCredits)
            ret += (ret.empty()) ? "aggregate_credits" : ",aggregate_credits";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Credits)
            ret += (ret.empty()) ? "credits" : ",credits";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::AlternativeTitles)
            ret += (ret.empty()) ? "alternative_titles" : ",alternative_titles";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Changes)
            ret += (ret.empty()) ? "changes" : ",changes";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::EpisodeGroups)
            ret += (ret.empty()) ? "episode_groups" : ",episode_groups";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::ExternalIds)
            ret += (ret.empty()) ? "external_ids" : ",external_ids";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Images)
            ret += (ret.empty()) ? "images" : ",images";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Keywords)
            ret += (ret.empty()) ? "keywords" : ",keywords";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Latest)
            ret += (ret.empty()) ? "latest" : ",latest";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Lists)
            ret += (ret.empty()) ? "lists" : ",lists";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Recommendations)
            ret += (ret.empty()) ? "recommendations" : ",recommendations";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Reviews)
            ret += (ret.empty()) ? "reviews" : ",reviews";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::ScreenedTheatrically)
            ret += (ret.empty()) ? "screened_theatrically" : ",screened_theatrically";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Similar)
            ret += (ret.empty()) ? "similar" : ",similar";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::WatchProviders)
            ret += (ret.empty()) ? "watch_providers" : ",watch_providers";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Translations)
            ret += (ret.empty()) ? "translations" : ",translations";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::Videos)
            ret += (ret.empty()) ? "videos" : ",videos";
        MATCH_ENUM(appendToResponse, TVAppendToResponse::ContentRatings)
            ret += (ret.empty()) ? "content_ratings" : ",content_ratings";
        return ret;
    }
    std::string appendToResponseToStr(MovieAppendToResponse appendToResponse)
    {
        std::string ret{};
        if (appendToResponse == MovieAppendToResponse::None)
            return ret;
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::AccountStates)
            ret += (ret.empty()) ? "account_states" : ",account_states";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::AggregateCredits)
            ret += (ret.empty()) ? "aggregate_credits" : ",aggregate_credits";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Credits)
            ret += (ret.empty()) ? "credits" : ",credits";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::AlternativeTitles)
            ret += (ret.empty()) ? "alternative_titles" : ",alternative_titles";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Changes)
            ret += (ret.empty()) ? "changes" : ",changes";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::ExternalIds)
            ret += (ret.empty()) ? "external_ids" : ",external_ids";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Images)
            ret += (ret.empty()) ? "images" : ",images";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Keywords)
            ret += (ret.empty()) ? "keywords" : ",keywords";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Latest)
            ret += (ret.empty()) ? "latest" : ",latest";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Lists)
            ret += (ret.empty()) ? "lists" : ",lists";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Recommendations)
            ret += (ret.empty()) ? "recommendations" : ",recommendations";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Reviews)
            ret += (ret.empty()) ? "reviews" : ",reviews";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::ScreenedTheatrically)
            ret += (ret.empty()) ? "screened_theatrically" : ",screened_theatrically";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Similar)
            ret += (ret.empty()) ? "similar" : ",similar";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Translations)
            ret += (ret.empty()) ? "translations" : ",translations";
        MATCH_ENUM(appendToResponse, MovieAppendToResponse::Videos)
            ret += (ret.empty()) ? "videos" : ",videos";
        return ret;
    }
    std::string appendToResponseToStr(TVSeasonAppendToResponse appendToResponse)
    {
        std::string ret{};
        if (appendToResponse == TVSeasonAppendToResponse::None)
            return ret;
        MATCH_ENUM(appendToResponse, TVSeasonAppendToResponse::AccountStates)
            ret += (ret.empty()) ? "account_states" : ",account_states";
        MATCH_ENUM(appendToResponse, TVSeasonAppendToResponse::AggregateCredits)
            ret += (ret.empty()) ? "aggregate_credits" : ",aggregate_credits";
        MATCH_ENUM(appendToResponse, TVSeasonAppendToResponse::Credits)
            ret += (ret.empty()) ? "credits" : ",credits";
        MATCH_ENUM(appendToResponse, TVSeasonAppendToResponse::Videos)
            ret += (ret.empty()) ? "videos" : ",videos";
        MATCH_ENUM(appendToResponse, TVSeasonAppendToResponse::Changes)
            ret += (ret.empty()) ? "changes" : ",changes";
        MATCH_ENUM(appendToResponse, TVSeasonAppendToResponse::ExternalIds)
            ret += (ret.empty()) ? "external_ids" : ",external_ids";
        MATCH_ENUM(appendToResponse, TVSeasonAppendToResponse::Images)
            ret += (ret.empty()) ? "images" : ",images";
        MATCH_ENUM(appendToResponse, TVSeasonAppendToResponse::Translations)
            ret += (ret.empty()) ? "translations" : ",translations";
        MATCH_ENUM(appendToResponse, TVSeasonAppendToResponse::WatchProviders)
            ret += (ret.empty()) ? "watch_providers" : ",watch_providers";
        return ret;
    }
    std::string appendToResponseToStr(EpisodeAppendToResponse appendToResponse)
    {
        std::string ret{};
        if (appendToResponse == EpisodeAppendToResponse::None)
            return ret;
        MATCH_ENUM(appendToResponse, EpisodeAppendToResponse::AccountStates)
            ret += (ret.empty()) ? "account_states" : ",account_states";
        MATCH_ENUM(appendToResponse, EpisodeAppendToResponse::Credits)
            ret += (ret.empty()) ? "credits" : ",credits";
        MATCH_ENUM(appendToResponse, EpisodeAppendToResponse::Videos)
            ret += (ret.empty()) ? "videos" : ",videos";
        MATCH_ENUM(appendToResponse, EpisodeAppendToResponse::Changes)
            ret += (ret.empty()) ? "changes" : ",changes";
        MATCH_ENUM(appendToResponse, EpisodeAppendToResponse::ExternalIds)
            ret += (ret.empty()) ? "external_ids" : ",external_ids";
        MATCH_ENUM(appendToResponse, EpisodeAppendToResponse::Images)
            ret += (ret.empty()) ? "images" : ",images";
        MATCH_ENUM(appendToResponse, EpisodeAppendToResponse::Translations)
            ret += (ret.empty()) ? "translations" : ",translations";
        return ret;
    }
    std::string appendToResponseToStr(PeopleAppendToResponse appendToResponse)
    {
        std::string ret{};
        if (appendToResponse == PeopleAppendToResponse::None)
            return ret;
        MATCH_ENUM(appendToResponse, PeopleAppendToResponse::Changes)
            ret += (ret.empty()) ? "changes" : ",changes";
        MATCH_ENUM(appendToResponse, PeopleAppendToResponse::CombinedCredits)
            ret += (ret.empty()) ? "combined_credits" : ",combined_credits";
        MATCH_ENUM(appendToResponse, PeopleAppendToResponse::Latest)
            ret += (ret.empty()) ? "latest" : ",latest";
        MATCH_ENUM(appendToResponse, PeopleAppendToResponse::ExternalIds)
            ret += (ret.empty()) ? "external_ids" : ",external_ids";
        MATCH_ENUM(appendToResponse, PeopleAppendToResponse::Images)
            ret += (ret.empty()) ? "images" : ",images";
        MATCH_ENUM(appendToResponse, PeopleAppendToResponse::Translations)
            ret += (ret.empty()) ? "translations" : ",translations";
        MATCH_ENUM(appendToResponse, PeopleAppendToResponse::TaggedImages)
            ret += (ret.empty()) ? "tagged_images" : ",tagged_images";
        MATCH_ENUM(appendToResponse, PeopleAppendToResponse::MovieCredits)
            ret += (ret.empty()) ? "movie_credits" : ",movie_credits";
        MATCH_ENUM(appendToResponse, PeopleAppendToResponse::TVCredits)
            ret += (ret.empty()) ? "tv_credits" : ",tv_credits";
        return ret;
    }

    std::string createParamString(const Languages language, const std::string& appendToResponse, bool firstParam)
    {
        std::string ret = createParamString(language, firstParam);
        if (!appendToResponse.empty())
            ret+="&append_to_response="+appendToResponse;
        return ret;
    }
    std::string createParamString(const Languages language, bool firstParam)
    {
        std::string ret = ((firstParam) ? std::string("?") : std::string("&")) + "language="+languageToStr(language);
        return ret;
    }

    std::string imageLanguagesToStr(std::vector<Languages> imageLanguages)
    {
        if (imageLanguages.empty())
            return "";
        std::string ret = languageToStr(*imageLanguages.begin());
        for (auto it = ++imageLanguages.begin(); it != imageLanguages.end(); ++it)
        {
            ret+="," + languageToStr(*it);
        }
        return ret;
    }

    #undef MATCH_ENUM
}