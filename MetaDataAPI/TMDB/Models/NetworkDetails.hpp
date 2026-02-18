#pragma once
#include "Models/ModelDefinition.hpp"
#include "Models/PartialCompanyNetwork.hpp"
namespace TMDBAPI::Models 
{
    struct NetworkDetails : PartialCompanyNetwork
    {
        std::string headquarters{};
        std::string homepage{};
        ModelClassDeffinitions(NetworkDetails)
    };
}