#pragma once
#include "RequestParams/BaseRequestParam.hpp"
#include <string>
namespace TMDBAPI::RequestParams
{
struct SingleRequestParam : BaseRequestParam
{
private:
    std::string value;
public:
    void setValue(std::string value);
    SingleRequestParam() = delete;
    SingleRequestParam(std::string name, std::string value);
    SingleRequestParam(const SingleRequestParam& other) = default;
    SingleRequestParam( SingleRequestParam&& other) = default;
    std::string str();
};
}