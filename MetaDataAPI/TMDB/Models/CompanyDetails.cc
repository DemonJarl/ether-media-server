#include "Models/CompanyDetails.hpp"
namespace TMDBAPI::Models
{
CompanyDetails::CompanyDetails(const Json::Value &json) : PartialCompanyNetwork(json)
{

}
}