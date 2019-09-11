#pragma once

#include "BaseDestructor.hpp"

namespace stdx::details {
    enum class EConstructorSelector { Disabled, Trivial, NonTrivial };

    template <typename T, typename E>
    constexpr EConstructorSelector SelectCopyConstructor() noexcept {
        if constexpr (VoidOrTriviallyCopyConstructible<T>() && TriviallyCopyConstructible<E>()) {
            return EConstructorSelector::Trivial;
        } else if constexpr (VoidOrCopyConstructible<T>() && CopyConstructible<E>()) {
            return EConstructorSelector::NonTrivial;
        } else {
            return EConstructorSelector::Disabled;
        }
    }

    template <typename T, typename E, EConstructorSelector = SelectCopyConstructor<T, E>()>
    struct BaseCopyConstructor : BaseDestructor<T, E> {
        using Super = BaseDestructor<T, E>;
        using Super::Super;

        BaseCopyConstructor() = default;

        BaseCopyConstructor(const BaseCopyConstructor&) = delete;

        BaseCopyConstructor(BaseCopyConstructor&&) = default;

        BaseCopyConstructor& operator=(const BaseCopyConstructor&) = default;

        BaseCopyConstructor& operator=(BaseCopyConstructor&&) = default;
    };

    template <typename T, typename E>
    struct BaseCopyConstructor<T, E, EConstructorSelector::Trivial> : BaseDestructor<T, E> {
        using Super = BaseDestructor<T, E>;
        using Super::Super;
    };

    template <typename T, typename E>
    struct BaseCopyConstructor<T, E, EConstructorSelector::NonTrivial> : BaseDestructor<T, E> {
        using Super = BaseDestructor<T, E>;
        using Super::Super;

        BaseCopyConstructor() = default;

        constexpr BaseCopyConstructor(const BaseCopyConstructor& Other) noexcept(
            VoidOrNothrowCopyConstructible<T>() && NothrowCopyConstructible<E>()) {
            if (Other.HasValue()) {
                if constexpr (!IsVoid<T>()) {
                    Super::ConstructValue(*Other);
                }
            } else {
                Super::ConstructUnexpected(Other.Error());
            }
            Super::bHasValue = Other.bHasValue;
        }

        BaseCopyConstructor(BaseCopyConstructor&&) = default;

        BaseCopyConstructor& operator=(const BaseCopyConstructor&) = default;

        BaseCopyConstructor& operator=(BaseCopyConstructor&&) = default;
    };
}
