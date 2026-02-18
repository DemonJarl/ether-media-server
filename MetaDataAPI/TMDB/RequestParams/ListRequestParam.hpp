#pragma once
#include "RequestParams/BaseRequestParam.hpp"
#include <cstddef>
#include <string>
#include <vector>
namespace TMDBAPI::RequestParams
{
struct ListRequestParam : BaseRequestParam
{
private:
    std::vector<std::string>values{};
public:
    void addValue(std::string value);
    void clearValues();
    ListRequestParam() = delete;
    ListRequestParam(const std::string name, const std::vector<std::string >values);
    ListRequestParam(const std::string name);
    ListRequestParam(const ListRequestParam& other) = default;
    ListRequestParam(ListRequestParam&& other) = default;
    std::string str();
    size_t size(){
        return values.size();
    }
};
}