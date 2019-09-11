#pragma once

#include "BaseCopy.hpp"

namespace stdx::details {
    template <typename T, typename E>
    constexpr EConstructorSelector SelectMoveConstructor() noexcept {
        if constexpr (VoidOrTriviallyMoveConstructible<T>() && TriviallyMoveConstructible<E>()) {
            return EConstructorSelector::Trivial;
        } else if constexpr (VoidOrMoveConstructible<T>() && MoveConstructible<E>()) {
            return EConstructorSelector::NonTrivial;
        } else {
            return EConstructorSelector::Disabled;
        }
    }

    template <typename T, typename E, EConstructorSelector = SelectMoveConstructor<T, E>()>
    struct BaseMoveConstructor : BaseCopyConstructor<T, E> {
        using Super = BaseCopyConstructor<T, E>;
        using Super::Super;

        BaseMoveConstructor() = default;

        BaseMoveConstructor(const BaseMoveConstructor&) = default;

        BaseMoveConstructor(BaseMoveConstructor&&) = delete;

        BaseMoveConstructor& operator=(const BaseMoveConstructor&) = default;

        BaseMoveConstructor& operator=(BaseMoveConstructor&&) = default;
    };

    template <typename T, typename E>
    struct BaseMoveConstructor<T, E, EConstructorSelector::Trivial> : BaseCopyConstructor<T, E> {
        using Super = BaseCopyConstructor<T, E>;
        using Super::Super;
    };

    template <typename T, typename E>
    struct BaseMoveConstructor<T, E, EConstructorSelector::NonTrivial> : BaseCopyConstructor<T, E> {
        using Super = BaseCopyConstructor<T, E>;
        using Super::Super;

        BaseMoveConstructor() = default;

        BaseMoveConstructor(const BaseMoveConstructor&) = default;

        BaseMoveConstructor(BaseMoveConstructor&& Other) noexcept(
            VoidOrNothrowMoveConstructible<T>() && NothrowMoveConstructible<E>()) {
            if (Other.HasValue()) {
                if constexpr (!IsVoid<T>()) {
                    Super::ConstructValue(std::move(*Other));
                }
            } else {
                Super::ConstructUnexpected(std::move(Other).Error());
            }
            Super::bHasValue = Other.bHasValue;
        }

        BaseMoveConstructor& operator=(const BaseMoveConstructor&) = default;

        BaseMoveConstructor& operator=(BaseMoveConstructor&&) = default;
    };
}