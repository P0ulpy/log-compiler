#pragma once

#include <type_traits>

template <typename ...Types>
struct TypeList 
{
    // TypeList utils
    
    // to std::variant

    struct ToVariant
    {
        using type = std::variant<Types...>;
    };

    using ToVariant_t = typename ToVariant::type;

    // Substraction
};

template <typename ...TypesTuple>
concept FundamentalTypes = (... && !std::is_fundamental_v<TypesTuple>);

template <typename ...Types>
requires FundamentalTypes<Types...>
struct FundamentalTypeList
    : public TypeList<Types...>
{};

// TypeList utils

template <typename... Types2>
struct TypeListSubtraction;

// Base case: Subtracting an empty list returns an empty list
template <>
struct TypeListSubtraction<TypeList<>, TypeList<>> {
    using type = TypeList<>;
};

// Recursive case: Subtract Types2 from Types1
template <typename Head, typename... Tail, typename... Types2>
struct TypeListSubtraction<TypeList<Head, Tail...>, TypeList<Types2...>> {
    using type = typename TypeListSubtraction<TypeList<Tail...>, TypeList<Types2...>>::type;
};

// If Head is present in Types2, exclude it
template <typename... Tail, typename... Types2>
struct TypeListSubtraction<TypeList<Tail...>, TypeList<Types2...>, Types2...> {
    using type = typename TypeListSubtraction<TypeList<Tail...>, TypeList<Types2...>>::type;
};

// If Head is not present in Types2, keep it in the result
template <typename Head, typename... Tail, typename... Types2>
struct TypeListSubtraction<TypeList<Head, Tail...>, TypeList<Types2...>, Types2...> {
    using type = typename TypeListSubtraction<TypeList<Head, Tail...>, TypeList<Types2...>>::type;
};