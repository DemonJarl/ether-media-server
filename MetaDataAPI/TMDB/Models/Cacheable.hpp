#pragma once
namespace TMDBAPI::Models
{
struct Cacheable // Для кэширования
{
    virtual ~Cacheable() = default;
};
}