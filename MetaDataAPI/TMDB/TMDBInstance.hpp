#pragma once
#include "BaseAPI.hpp"
#include "Endpoints/TVSeries.hpp"
#include <string>
namespace TMDBAPI
{

class TMDBInstance : BaseAPI
{
private:
public:
    // Endpoints::Movies TVSeries(int id);
    TMDBInstance() = default;
};

}