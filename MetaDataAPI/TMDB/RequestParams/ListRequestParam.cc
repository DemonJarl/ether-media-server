#include "RequestParams/ListRequestParam.hpp"
#include "RequestParams/BaseRequestParam.hpp"
#include <stdexcept>
#include <string>
#include <vector>
namespace TMDBAPI::RequestParams
{
void ListRequestParam::addValue(const std::string value)
{
    if (value.size() == 0)
        return;
    values.push_back(value);
}
void ListRequestParam::clearValues()
{
    values.clear();
}

ListRequestParam::ListRequestParam(const std::string name) : BaseRequestParam(name)
{

}

ListRequestParam::ListRequestParam(const std::string name, const std::vector<std::string> values) : BaseRequestParam(name)
{
    if (name.size() == 0)
        throw std::invalid_argument("Request parameter name cannot be empty");
    this->name = name;
}

std::string ListRequestParam::str()
{
    if (values.size() == 0)
        return "";
    std::string ret = name+"=";
    ret += values[0];
    for (auto iter = ++begin(values); iter!=end(values); ++iter)
        ret += "," + *iter;
    return ret;
}
}