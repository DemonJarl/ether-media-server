
#include "BaseRequestParam.hpp"
#include <stdexcept>
#include <string>

namespace TMDBAPI::RequestParams
{
void BaseRequestParam::setName(const std::string name)
{
    if (name.empty())
        throw std::invalid_argument("Request parameter name cannot be empty");
    this->name = name;
}
BaseRequestParam::BaseRequestParam(const std::string name)
{
    if (name.empty())
        throw std::invalid_argument("Request parameter name cannot be empty");
    this->name = name;
}
std::string BaseRequestParam::str()
{
    return "";
}
}