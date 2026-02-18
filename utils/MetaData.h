#pragma once
#include "MediaEnums.h"
#include <string>
#include <vector>

struct TitleDescription
{
    std::string text;
    Language language;
};

struct MetaData
{
    std::vector<TitleDescription> titles;
    std::vector<TitleDescription> descriptions;
    
};