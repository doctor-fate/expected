#pragma once

#include "BaseCopyAssign.hpp"

namespace stdx::details {
    template <typename T, typename E>
    bool constexpr EnableMoveAssignment() noexcept {
        return And<
            Or<IsVoid<T>,
               And<MoveAssignable<T>, MoveConstructible<T>, Or<NothrowMoveConstructible<T>, NothrowMoveConstructible<E>>>>,
            MoveAssignable<E>,
            MoveConstructible<E>>();
    }

    template <typename T, typename E, bool Enable = EnableMoveAssignment<T, E>()>
    struct BaseMoveAssignment : BaseCopyAssignment<T, E> {
        using Super = BaseCopyAssignment<T, E>;
        using Super::Super;

        BaseMoveAssignment() = default;

        BaseMoveAssignment(const BaseMoveAssignment&) = default;

        BaseMoveAssignment(BaseMoveAssignment&&) = default;

        BaseMoveAssignment& operator=(const BaseMoveAssignment&) = default;

        BaseMoveAssignment& operator=(BaseMoveAssignment&&) = delete;
    };

    template <typename T, typename E>
    struct BaseMoveAssignment<T, E, true> : BaseCopyAssignment<T, E> {
        using Super = BaseCopyAssignment<T, E>;
        using Super::Super;

        BaseMoveAssignment() = default;

        BaseMoveAssignment(const BaseMoveAssignment&) = default;

        BaseMoveAssignment(BaseMoveAssignment&&) = default;

        BaseMoveAssignment& operator=(const BaseMoveAssignment&) = default;

        BaseMoveAssignment& operator=(BaseMoveAssignment&& Other) noexcept(
            And<VoidOrNothrowMoveConstructible<T>,
                NothrowMoveConstructible<E>,
                VoidOrNothrowMoveAssignable<T>,
                NothrowMoveAssignable<E>>()) {
            if (Other.HasValue()) {
                if (Super::HasValue()) {
                    if constexpr (!IsVoid<T>()) {
                        Super::AssignValue(std::move(*Other));
                    }
                } else {
                    if constexpr (IsVoid<T>()) {
                        Super::DestroyUnexpected();
                    } else if constexpr (NothrowMoveConstructible<T>()) {
                        Super::DestroyUnexpected();
                        Super::ConstructValue(std::move(*Other));
                    } else {
                        Unexpected<E> Tmp(std::move(Super::Error()));
                        Super::DestroyUnexpected();
                        try {
                            Super::ConstructValue(std::move(*Other));
                        } catch (...) {
                            Super::ConstructUnexpected(std::move(Tmp));
                            throw;
                        }
                    }
                }
            } else {
                if (Super::HasValue()) {
                    if constexpr (IsVoid<T>()) {
                        Super::ConstructUnexpected(std::move(Other).Error());
                    } else if constexpr (NothrowMoveConstructible<E>()) {
                        Super::DestroyValue();
                        Super::ConstructUnexpected(std::move(Other).Error());
                    } else {
                        T Tmp(std::move(**this));
                        Super::DestroyValue();
                        try {
                            Super::ConstructUnexpected(std::move(Other).Error());
                        } catch (...) {
                            Super::ConstructValue(std::move(Tmp));
                            throw;
                        }
                    }
                } else {
                    Super::AssignUnexpected(Unexpected<E>(std::move(Other).Error()));
                }
            }
            Super::bHasValue = Other.bHasValue;
            return *this;
        }
    };
}