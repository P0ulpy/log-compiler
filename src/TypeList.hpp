#pragma once

#include <type_traits>

template <typename ...Types>
struct TypeList {};

template <typename ...TypesTuple>
concept FundamentalTypes = (... && !std::is_fundamental_v<TypesTuple>);

template <typename ...Types>
requires FundamentalTypes<Types...>
struct FundamentalTypeList
    : TypeList<Types...>
{};

// TypeList utils

template<typename ...Types>
struct TypeListToVariant;

template<typename ...Types>
struct TypeListToVariant<TypeList<Types...>>
{
    using type = std::variant<Types...>;
};

template<typename ...Types>
struct TypeListToVariant<FundamentalTypeList<Types...>> { using type = std::variant<Types...>; };

template<typename ...Types>
using TypeListToVariant_t = typename TypeListToVariant<Types...>::type;