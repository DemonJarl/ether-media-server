#pragma once
#include <type_traits>
#define FLAG_ENUM(_enumName)\
inline _enumName operator|(_enumName lhs, _enumName rhs)\
{\
    return static_cast<_enumName>(static_cast<std::underlying_type<_enumName>::type>(lhs) | static_cast<std::underlying_type<_enumName>::type>(rhs));\
}\
inline _enumName operator&(_enumName lhs, _enumName rhs)\
{\
    return static_cast<_enumName>(static_cast<std::underlying_type<_enumName>::type>(lhs) & static_cast<std::underlying_type<_enumName>::type>(rhs));\
}\
inline _enumName operator!(_enumName lhs)\
{\
    return static_cast<_enumName>(!static_cast<std::underlying_type<_enumName>::type>(lhs));\
}\


#define MATCH_ENUM(_enumVal, _match)\
        if((_enumVal & _match) == _match)\