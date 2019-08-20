#pragma once

#include <type_traits>
#include <utility>

#include <Expected/Tags.hpp>

namespace stdx {
    template <typename E>
    class Unexpected;

    template <typename T, typename E>
    class Expected;
}

namespace stdx::details {
    template <typename T>
    using RemoveCVRef = std::remove_const_t<std::remove_reference_t<T>>;

    template <typename T>
    using Decay = std::decay_t<T>;

    template <typename T>
    using IsVoid = std::is_void<T>;

    template <typename T>
    using IsScalar = std::is_scalar<T>;

    template <bool Value>
    using BoolConstant = std::bool_constant<Value>;

    template <typename... Ts>
    using And = std::conjunction<Ts...>;

    template <typename... Ts>
    using Or = std::disjunction<Ts...>;

    template <typename T>
    using Not = std::negation<T>;

    template <typename T1, typename T2>
    using Same = std::is_same<T1, T2>;

    template <typename T>
    using DefaultConstructible = std::is_default_constructible<T>;

    template <typename T>
    using VoidOrDefaultConstructible = Or<IsVoid<T>, DefaultConstructible<T>>;

    template <typename T>
    using NothrowDefaultConstructible = std::is_nothrow_default_constructible<T>;

    template <typename T>
    using VoidOrNothrowDefaultConstructible = Or<IsVoid<T>, NothrowDefaultConstructible<T>>;

    template <typename T>
    using TriviallyDestructible = std::is_trivially_destructible<T>;

    template <typename T>
    using VoidOrTriviallyDestructible = Or<IsVoid<T>, TriviallyDestructible<T>>;

    template <typename T, typename... Ts>
    using Constructible = std::is_constructible<T, Ts...>;

    template <typename T, typename... Ts>
    using NothrowConstructible = std::is_nothrow_constructible<T, Ts...>;

    template <typename T>
    using CopyConstructible = std::is_copy_constructible<T>;

    template <typename T>
    using VoidOrCopyConstructible = Or<IsVoid<T>, CopyConstructible<T>>;

    template <typename T>
    using NothrowCopyConstructible = std::is_nothrow_copy_constructible<T>;

    template <typename T>
    using VoidOrNothrowCopyConstructible = Or<IsVoid<T>, NothrowCopyConstructible<T>>;

    template <typename T>
    using TriviallyCopyConstructible = std::is_trivially_copy_constructible<T>;

    template <typename T>
    using VoidOrTriviallyCopyConstructible = Or<IsVoid<T>, TriviallyCopyConstructible<T>>;

    template <typename T>
    using MoveConstructible = std::is_move_constructible<T>;

    template <typename T>
    using VoidOrMoveConstructible = Or<IsVoid<T>, MoveConstructible<T>>;

    template <typename T>
    using NothrowMoveConstructible = std::is_nothrow_move_constructible<T>;

    template <typename T>
    using VoidOrNothrowMoveConstructible = Or<IsVoid<T>, NothrowMoveConstructible<T>>;

    template <typename T>
    using TriviallyMoveConstructible = std::is_trivially_move_constructible<T>;

    template <typename T>
    using VoidOrTriviallyMoveConstructible = Or<IsVoid<T>, TriviallyMoveConstructible<T>>;

    template <typename T, typename U>
    using Assignable = std::is_assignable<T, U>;

    template <typename T, typename U>
    using NothrowAssignable = std::is_nothrow_assignable<T, U>;

    template <typename T>
    using CopyAssignable = std::is_copy_assignable<T>;

    template <typename T>
    using NothrowCopyAssignable = std::is_nothrow_copy_assignable<T>;

    template <typename T>
    using VoidOrNothrowCopyAssignable = Or<IsVoid<T>, NothrowCopyAssignable<T>>;

    template <typename T>
    using MoveAssignable = std::is_move_assignable<T>;

    template <typename T>
    using NothrowMoveAssignable = std::is_nothrow_move_assignable<T>;

    template <typename T>
    using VoidOrNothrowMoveAssignable = Or<IsVoid<T>, NothrowMoveAssignable<T>>;

    template <typename T>
    using Swappable = std::is_swappable<T>;

    template <typename T>
    using NothrowSwappable = std::is_nothrow_swappable<T>;

    template <typename T>
    using VoidOrNothrowSwappable = Or<IsVoid<T>, NothrowSwappable<T>>;

    template <typename From, typename To>
    using Convertible = std::is_convertible<From, To>;

    template <typename Condition, typename T1, typename T2>
    using Conditional = std::conditional_t<bool(Condition::value), T1, T2>;

    template <typename E>
    struct IsUnexpectedSpecialization : std::false_type {};

    template <typename E>
    struct IsUnexpectedSpecialization<Unexpected<E>> : std::true_type {};

    template <typename T>
    struct IsExpectedSpecialization : std::false_type {};

    template <typename T, typename E>
    struct IsExpectedSpecialization<Expected<T, E>> : std::true_type {};

    template <typename E>
    using ValidUnexpectedSpecialization =
        And<std::is_object<E>, Not<std::is_array<E>>, Same<E, std::remove_cv_t<E>>, Not<IsUnexpectedSpecialization<E>>>;

    template <typename T, typename E, typename K = std::remove_cv_t<T>>
    using ValidExpectedSpecialization =
        And<Or<IsVoid<T>, std::is_destructible<T>>,
            std::is_destructible<E>,
            ValidUnexpectedSpecialization<E>,
            Not<
                Or<std::is_reference<T>,
                   std::is_function<T>,
                   Same<std::in_place_t, K>,
                   Same<unexpect_t, K>,
                   IsExpectedSpecialization<K>>>>;
}
