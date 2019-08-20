#pragma once

#include "Traits.hpp"

namespace stdx::details {
    template <typename E, typename Err>
    struct ConstructibleFromUnexpected :
        Or<Constructible<E, Unexpected<Err>&>,
           Constructible<E, const Unexpected<Err>&>,
           Constructible<E, Unexpected<Err>>,
           Constructible<E, const Unexpected<Err>>,
           Convertible<Unexpected<Err>&, E>,
           Convertible<const Unexpected<Err>&, E>,
           Convertible<Unexpected<Err>, E>,
           Convertible<const Unexpected<Err>&, E>> {};

    template <typename E, typename G, typename K = RemoveCVRef<G>>
    struct ConstructibleFromG : And<Constructible<E, G>, Not<Same<K, std::in_place_t>>, Not<IsUnexpectedSpecialization<K>>> {};

    template <typename E, typename Err>
    struct CopyConstructibleFromUnexpected {
        using Result = And<Not<ConstructibleFromUnexpected<E, Err>>, Constructible<E, const Err&>>;

        static constexpr bool Implicit = Result() && Convertible<const Err&, E>();
        static constexpr bool Explicit = Result() && !Convertible<const Err&, E>();
        static constexpr bool Nothrow = NothrowConstructible<E, const Err&>();
    };

    template <typename E, typename Err>
    struct MoveConstructibleFromUnexpected {
        using Result = And<Not<ConstructibleFromUnexpected<E, Err>>, Constructible<E, Err&&>>;

        static constexpr bool Implicit = Result() && Convertible<Err&&, E>();
        static constexpr bool Explicit = Result() && !Convertible<Err&&, E>();
        static constexpr bool Nothrow = NothrowConstructible<E, Err&&>();
    };
}