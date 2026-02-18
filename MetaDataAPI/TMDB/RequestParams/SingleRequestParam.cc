#include "SingleRequestParam.hpp"
#include "RequestParams/BaseRequestParam.hpp"
#include <stdexcept>
namespace TMDBAPI::RequestParams
{
void SingleRequestParam::setValue(std::string value)
{
    if (value.size() == 0)
        throw std::invalid_argument("Requset parameter value cannot be empty");
    this->value = value;
}

SingleRequestParam::SingleRequestParam(std::string name, std::string value) : BaseRequestParam(name)
{
    if (name.size() == 0)
        throw std::invalid_argument("Requset parameter name cannot be empty");
    this->name = name;
    if (value.size() == 0)
        throw std::invalid_argument("Requset parameter value cannot be empty");
    this->value = value;
}

std::string SingleRequestParam::str()
{
    return name+"="+value;
}
}