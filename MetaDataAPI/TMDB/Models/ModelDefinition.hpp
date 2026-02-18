#pragma once
#include <json/value.h>

#define ModelClassDeffinitions(_classname) \
public:\
    _classname() = default; \
    _classname(const Json::Value& json); \
    _classname(const _classname& other) = default; \
    _classname(_classname&& other) = default; \
    _classname& operator=(_classname&& other) = default; \
    _classname& operator=(const _classname& other) = default;