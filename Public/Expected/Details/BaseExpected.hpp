#pragma once

#include "BaseMoveAssign.hpp"
#include "ExpectedTraits.hpp"

namespace stdx::details {

    template <typename T, typename E>
    class BaseExpected : public BaseMoveAssignment<T, E> {
        using Super = BaseMoveAssignment<T, E>;

    public:
        constexpr const T& Value() const& {
            if (!Super::HasValue()) {
                throw BadExpectedAccess(Super::Error());
            }
            return **this;
        }

        constexpr T& Value() & {
            if (!Super::HasValue()) {
                throw BadExpectedAccess(Super::Error());
            }
            return **this;
        }

        constexpr const T&& Value() const&& {
            if (!Super::HasValue()) {
                throw BadExpectedAccess(std::move(Super::Error()));
            }
            return std::move(**this);
        }

        constexpr T&& Value() && {
            if (!Super::HasValue()) {
                throw BadExpectedAccess(std::move(Super::Error()));
            }
            return std::move(**this);
        }

        template <typename U>
        [[nodiscard]] constexpr T ValueOr(U&& Default) const& {
            return Super::HasValue() ? **this : static_cast<T>(std::forward<U>(Default));
        }

        template <typename U>
        [[nodiscard]] constexpr T ValueOr(U&& Default) && {
            return Super::HasValue() ? std::move(**this) : static_cast<T>(std::forward<U>(Default));
        }

        template <typename... Ts>
        T& Emplace(Ts&&... Args) noexcept(noexcept(std::declval<BaseExpected<T, E>>().DoEmplace(std::forward<Ts>(Args)...))) {
            return DoEmplace(std::forward<Ts>(Args)...);
        }

        template <typename U, typename... Ts>
        T& Emplace(std::initializer_list<U> List, Ts&&... Args) noexcept(
            noexcept(std::declval<BaseExpected<T, E>>().DoEmplace(List, std::forward<Ts>(Args)...))) {
            return DoEmplace(List, std::forward<Ts>(Args)...);
        }

    protected:
        using Super::Super;

    private:
        template <typename... Ts, typename std::enable_if_t<EmplacebleFromTs<T, E, Ts...>::value, int> = 0>
        T& DoEmplace(Ts&&... Args) noexcept(NothrowConstructible<T, Ts...>()) {
            if (Super::HasValue()) {
                if constexpr (NothrowConstructible<T, Ts...>()) {
                    Super::DestroyValue();
                    Super::ConstructValue(std::forward<Ts>(Args)...);
                } else {
                    Super::AssignValue(T(std::forward<Ts>(Args)...));
                }
            } else {
                if constexpr (NothrowConstructible<T, Ts...>()) {
                    Super::DestroyUnexpected();
                    Super::ConstructValue(std::forward<Ts>(Args)...);
                } else if constexpr (NothrowMoveConstructible<T>()) {
                    T Tmp(std::forward<Ts>(Args)...);
                    Super::DestroyUnexpected();
                    Super::ConstructValue(std::move(Tmp));
                } else {
                    Unexpected<E> Tmp(std::move(Super::Error()));
                    Super::DestroyUnexpected();
                    try {
                        Super::ConstructValue(std::forward<Ts>(Args)...);
                    } catch (...) {
                        Super::ConstructUnexpected(std::move(Tmp));
                        throw;
                    }
                }
            }
            Super::bHasValue = true;
            return **this;
        }
    };

#define _BASE_EXPECTED_VOID(Type)                                                                                                \
    template <typename E>                                                                                                        \
    class BaseExpected<Type, E> : public BaseMoveAssignment<void, E> {                                                           \
        using Super = BaseMoveAssignment<void, E>;                                                                               \
                                                                                                                                 \
    public:                                                                                                                      \
        constexpr void Value() const& {                                                                                          \
            if (!Super::HasValue()) {                                                                                            \
                throw BadExpectedAccess(Super::Error());                                                                         \
            }                                                                                                                    \
        }                                                                                                                        \
                                                                                                                                 \
        constexpr void Value() & {                                                                                               \
            if (!Super::HasValue()) {                                                                                            \
                throw BadExpectedAccess(Super::Error());                                                                         \
            }                                                                                                                    \
        }                                                                                                                        \
                                                                                                                                 \
        constexpr void Value() const&& {                                                                                         \
            if (!Super::HasValue()) {                                                                                            \
                throw BadExpectedAccess(std::move(Super::Error()));                                                              \
            }                                                                                                                    \
        }                                                                                                                        \
                                                                                                                                 \
        constexpr void Value() && {                                                                                              \
            if (!Super::HasValue()) {                                                                                            \
                throw BadExpectedAccess(std::move(Super::Error()));                                                              \
            }                                                                                                                    \
        }                                                                                                                        \
                                                                                                                                 \
        void Emplace() noexcept {                                                                                                \
            if (!Super::HasValue()) {                                                                                            \
                Super::DestroyUnexpected();                                                                                      \
            }                                                                                                                    \
            Super::bHasValue = true;                                                                                             \
        }                                                                                                                        \
                                                                                                                                 \
    protected:                                                                                                                   \
        using Super::Super;                                                                                                      \
    }

    _BASE_EXPECTED_VOID(void);
    _BASE_EXPECTED_VOID(const void);
    _BASE_EXPECTED_VOID(volatile void);
    _BASE_EXPECTED_VOID(const volatile void);

#undef _BASE_EXPECTED_VOID
}