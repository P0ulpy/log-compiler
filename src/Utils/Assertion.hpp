#pragma once

#include <type_traits>

template<typename T>
struct assert_false : std::false_type {};

template<typename T>
constexpr bool assert_false_v = assert_false<T>::value;
