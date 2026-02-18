#pragma once
#include "Models/Cacheable.hpp"
#include "Models/MediaImages.hpp"
#include "PersonDetails.hpp"
#include "PersonExternalIds.hpp"
#include <json/value.h>
#include <optional>
namespace TMDBAPI::Models
{
    class Person : public Cacheable
    {
        int id{};
    public:
        PersonDetails details;
        std::optional<MediaImages> images{};
        std::optional<PersonExternalIds> externalIDs;
        ModelClassDeffinitions(Person)
    };
}