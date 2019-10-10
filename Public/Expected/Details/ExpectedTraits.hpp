#pragma once

#include "Traits.hpp"

namespace stdx::details {
    template <typename T, typename U, typename G>
    struct ConstructibleFromExpected :
        Or<Constructible<T, const Expected<U, G>&>,
           Constructible<T, Expected<U, G>&>,
           Constructible<T, const Expected<U, G>&&>,
           Constructible<T, Expected<U, G>&&>,
           Convertible<const Expected<U, G>&, T>,
           Convertible<Expected<U, G>&, T>,
           Convertible<const Expected<U, G>&&, T>,
           Convertible<Expected<U, G>&&, T>> {};

    template <typename T, typename E, typename U, typename G, bool = IsVoid<T>() || IsVoid<U>()>
    struct CopyConstructibleFromExpected {
        using Result = And<Constructible<T, const U&>, Constructible<E, const G&>, Not<ConstructibleFromExpected<T, U, G>>>;

        static constexpr bool Implicit = Result() && (Convertible<const U&, T>() && Convertible<const G&, E>());
        static constexpr bool Explicit = Result() && !(Convertible<const U&, T>() && Convertible<const G&, E>());
        static constexpr bool Nothrow = NothrowConstructible<T, const U&>() && NothrowConstructible<E, const G&>();
    };

    template <typename T, typename E>
    struct CopyConstructibleFromExpected<T, E, T, E, false> {};

    template <typename T, typename E, typename U, typename G>
    struct CopyConstructibleFromExpected<T, E, U, G, true> {
        using Result = And<IsVoid<T>, IsVoid<U>, Constructible<E, const G&>>;

        static constexpr bool Implicit = Result() && Convertible<const G&, E>();
        static constexpr bool Explicit = Result() && !Convertible<const G&, E>();
        static constexpr bool Nothrow = NothrowConstructible<E, const G&>();
    };

    template <typename T, typename E, typename U, typename G, bool = IsVoid<T>() || IsVoid<U>()>
    struct MoveConstructibleFromExpected {
        using Result = And<Constructible<T, U&&>, Constructible<E, G&&>, Not<ConstructibleFromExpected<T, U, G>>>;

        static constexpr bool Implicit = Result() && (Convertible<U&&, T>() && Convertible<G&&, E>());
        static constexpr bool Explicit = Result() && !(Convertible<U&&, T>() && Convertible<G&&, E>());
        static constexpr bool Nothrow = NothrowConstructible<T, U&&>() && NothrowConstructible<E, G&&>();
    };

    template <typename T, typename E>
    struct MoveConstructibleFromExpected<T, E, T, E, false> {};

    template <typename T, typename E, typename U, typename G>
    struct MoveConstructibleFromExpected<T, E, U, G, true> {
        using Result = And<IsVoid<T>, IsVoid<U>, Constructible<E, G&&>>;

        static constexpr bool Implicit = Result() && Convertible<G&&, E>();
        static constexpr bool Explicit = Result() && !Convertible<G&&, E>();
        static constexpr bool Nothrow = NothrowConstructible<E, G&&>();
    };

    template <typename T, typename E, typename U, typename K = RemoveCVRef<U>>
    struct ConstructibleFromU {
        using Result =
            And<Not<IsVoid<T>>,
                Not<Same<K, std::in_place_t>>,
                Not<Same<K, Expected<T, E>>>,
                Not<IsUnexpectedSpecialization<K>>,
                Constructible<T, U>>;

        static constexpr bool Implicit = Result() && Convertible<U, T>();
        static constexpr bool Explicit = Result() && !Convertible<U, T>();
        static constexpr bool Nothrow = NothrowConstructible<T, U>();
    };

    template <typename T, typename... Ts>
    struct ConstructibleInPlace : Or<And<IsVoid<T>, BoolConstant<sizeof...(Ts) == 0>>, Constructible<T, Ts...>> {};

    template <typename T, typename E, typename U, typename K = RemoveCVRef<U>>
    struct AssignableFromU :
        And<Not<IsUnexpectedSpecialization<K>>,
            Not<And<IsScalar<T>, Same<T, Decay<U>>>>,
            Constructible<T, U>,
            Assignable<T&, U>,
            NothrowMoveConstructible<E>> {};

    template <typename T, typename E, typename... Ts>
    struct EmplacebleFromTs :
        Or<NothrowConstructible<T, Ts...>,
           And<Constructible<T, Ts...>, MoveAssignable<T>, Or<NothrowMoveConstructible<T>, NothrowMoveConstructible<E>>>> {};
}
