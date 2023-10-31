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

    // Concat

    template <typename ...OtherTypes>
    struct Concat;

    template <typename ...OtherTypes>
    struct Concat<TypeList<OtherTypes...>>
    {
        using type = TypeList<Types..., OtherTypes...>;
    };

    template <typename ...OtherTypes>
    using Concat_t = typename Concat<OtherTypes...>::type;
};

template <typename ...TypesTuple>
concept FundamentalTypes = (... && !std::is_fundamental_v<TypesTuple>);

template <typename ...Types>
requires FundamentalTypes<Types...>
struct FundamentalTypeList
    : public TypeList<Types...>
{};

// TypeList utils