
#pragma once
#include <string>
namespace TMDBAPI::RequestParams
{
struct BaseRequestParam
{
protected:
    std::string name{};
public:
    BaseRequestParam() = delete;
    BaseRequestParam(const std::string name);
    BaseRequestParam(const BaseRequestParam& other) = default;
    BaseRequestParam( BaseRequestParam&& other) = default;
    void setName(const std::string name);
    virtual std::string str();
};
}