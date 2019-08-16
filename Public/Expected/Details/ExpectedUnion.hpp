#pragma once

#include <Expected/Unexpected.hpp>

#include "Traits.hpp"

namespace stdx::details
{
    struct valueless_t
    {
        constexpr explicit valueless_t() = default;
    };

    constexpr valueless_t valueless;

    template <typename T, typename E, bool = And<VoidOrTriviallyDestructible<T>, TriviallyDestructible<E>>()>
    union ExpectedUnion;

#define _EXPECTED_UNION(TriviallyDestructible, Destructor)                                                                                           \
    template <typename T, typename E>                                                                                                                \
    union ExpectedUnion<T, E, TriviallyDestructible> {                                                                                               \
        explicit constexpr ExpectedUnion(valueless_t) noexcept : __Dummy()                                                                           \
        {                                                                                                                                            \
        }                                                                                                                                            \
                                                                                                                                                     \
        template <typename... Ts>                                                                                                                    \
        explicit constexpr ExpectedUnion(std::in_place_index_t<0>, Ts&&... Args) noexcept(NothrowConstructible<T, Ts...>::value) :                   \
            Value(std::forward<Ts>(Args)...)                                                                                                         \
        {                                                                                                                                            \
        }                                                                                                                                            \
                                                                                                                                                     \
        template <typename... Ts>                                                                                                                    \
        explicit constexpr ExpectedUnion(std::in_place_index_t<1>, Ts&&... Args) noexcept(NothrowConstructible<Unexpected<E>, Ts...>::value) :       \
            Unex(std::forward<Ts>(Args)...)                                                                                                          \
        {                                                                                                                                            \
        }                                                                                                                                            \
                                                                                                                                                     \
        Destructor;                                                                                                                                  \
                                                                                                                                                     \
        valueless_t __Dummy;                                                                                                                         \
        T Value;                                                                                                                                     \
        Unexpected<E> Unex;                                                                                                                          \
    };                                                                                                                                               \
                                                                                                                                                     \
    template <typename E>                                                                                                                            \
    union ExpectedUnion<void, E, TriviallyDestructible> {                                                                                            \
        explicit constexpr ExpectedUnion(valueless_t) noexcept : __Dummy()                                                                           \
        {                                                                                                                                            \
        }                                                                                                                                            \
                                                                                                                                                     \
        explicit constexpr ExpectedUnion(std::in_place_index_t<0>) noexcept : ExpectedUnion(valueless)                                               \
        {                                                                                                                                            \
        }                                                                                                                                            \
                                                                                                                                                     \
        template <typename... Ts>                                                                                                                    \
        explicit constexpr ExpectedUnion(std::in_place_index_t<1>, Ts&&... Args) noexcept(NothrowConstructible<Unexpected<E>, Ts...>::value) :       \
            Unex(std::forward<Ts>(Args)...)                                                                                                          \
        {                                                                                                                                            \
        }                                                                                                                                            \
                                                                                                                                                     \
        Destructor;                                                                                                                                  \
                                                                                                                                                     \
        valueless_t __Dummy;                                                                                                                         \
        Unexpected<E> Unex;                                                                                                                          \
    };

    _EXPECTED_UNION(true, ~ExpectedUnion() = default)
    _EXPECTED_UNION(false, ~ExpectedUnion(){})

#undef _EXPECTED_UNION
}