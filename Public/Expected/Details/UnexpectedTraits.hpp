#pragma once

#include "Traits.hpp"

namespace stdx::details {
    template <typename E, typename Err>
    struct ConstructibleFromUnexpected :
        Or<Constructible<E, Unexpected<Err>&>,
           Constructible<E, const Unexpected<Err>&>,
           Constructible<E, Unexpected<Err>&&>,
           Constructible<E, const Unexpected<Err>&&>,
           Convertible<Unexpected<Err>&, E>,
           Convertible<const Unexpected<Err>&, E>,
           Convertible<Unexpected<Err>&&, E>,
           Convertible<const Unexpected<Err>&&, E>> {};

    template <typename E, typename G, typename K = RemoveCVRef<G>>
    struct ConstructibleFromG : And<Not<Same<K, Unexpected<E>>>, Constructible<E, G>, Not<Same<K, std::in_place_t>>> {};

    template <typename E, typename Err>
    struct CopyConstructibleFromUnexpected {
        using Result = And<Constructible<E, const Err&>, Not<ConstructibleFromUnexpected<E, Err>>>;

        static constexpr bool Implicit = Result() && Convertible<const Err&, E>();
        static constexpr bool Explicit = Result() && !Convertible<const Err&, E>();
        static constexpr bool Nothrow = NothrowConstructible<E, const Err&>();
    };

    template <typename E>
    struct CopyConstructibleFromUnexpected<E, E> {};

    template <typename E, typename Err>
    struct MoveConstructibleFromUnexpected {
        using Result = And<Constructible<E, Err&&>, Not<ConstructibleFromUnexpected<E, Err>>>;

        static constexpr bool Implicit = Result() && Convertible<Err&&, E>();
        static constexpr bool Explicit = Result() && !Convertible<Err&&, E>();
        static constexpr bool Nothrow = NothrowConstructible<E, Err&&>();
    };

    template <typename E>
    struct MoveConstructibleFromUnexpected<E, E> {};
}