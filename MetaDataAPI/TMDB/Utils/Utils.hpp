#pragma once
#include "MetaDataAPI/TMDB/Enums/Enums.hpp"
#include "boost/algorithm/string/replace.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <string>
#include <vector>
namespace TMDBAPI::Utils
{
    std::string appendToResponseToStr(TVAppendToResponse appendToResponse);
    std::string appendToResponseToStr(MovieAppendToResponse appendToResponse);
    std::string appendToResponseToStr(TVSeasonAppendToResponse appendToResponse);
    std::string appendToResponseToStr(EpisodeAppendToResponse appendToResponse);
    std::string appendToResponseToStr(PeopleAppendToResponse appendToResponse);

    std::string imageLanguagesToStr(std::vector<Languages> imageLanguages);

    std::string createParamString(const Languages language, const std::string& appendToResponse, bool firstParam = true);
    std::string createParamString(const Languages language, bool firstParam = true);
    inline std::string boolToString(bool val)
    {
        return (val) ? "true" : "false";
    }

    inline bool isStringEmptyCopy(const std::string& str)
    {
        return boost::trim_copy(str).size() == 0;
    }

    inline bool isStringEmpty(std::string& str)
    {
        boost::trim(str);
        return str.size() == 0;
    }
    inline std::string sanitizeURLCopy(const std::string& url)
    {
        std::string ret = boost::trim_copy(url);
        boost::replace_all(ret, " ", "%20");
        return ret;
    }
    inline void sanitizeURL(std::string& url)
    {
        boost::trim(url);
        boost::replace_all(url, " ", "%20");
    }
}