#pragma once

#include <type_traits>

template <typename T>
concept Enum = std::is_enum_v<T>;

template <Enum T> 
auto enumToInt(T enumValue) -> decltype(std::underlying_type_t<T>())
{
    return static_cast<std::underlying_type_t<T>>(enumValue);
}

template <Enum T>
bool hasFlag(const T enumValue, const T flag)
{
    return (enumValue & flag) == flag;
}

template <Enum T>
T removeFlag(const T enumValue, const T flag)
{
    return enumValue & !flag;
}

// template <Enum T>
// auto removeFlag(const T enumValue, const T flag) -> decltype(std::underlying_type_t<T>())
// {
//     return enumToInt(enumValue & !flag);
// }
// template <typename E, typename T>
// E intToEnum(const T intVal)
// {
//     static_assert(std::is_integral_v<T>, "T должно быть целочисленным числом");
//     static_assert(std::is_enum_v<E>, "E должно быть перечислением");
//     return static_cast<E>(intVal);
// }