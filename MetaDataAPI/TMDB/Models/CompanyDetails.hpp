#pragma once
#include "Models/ModelDefinition.hpp"
#include "Models/PartialCompanyNetwork.hpp"
namespace TMDBAPI::Models
{
struct CompanyDetails : PartialCompanyNetwork
{
    std::string description{};
    std::string headquarters{};
    std::string parentCompany{};
    ModelClassDeffinitions(CompanyDetails)
};
}