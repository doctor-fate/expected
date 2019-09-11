#include <memory>
#include <new>
#include <utility>

#include <Expected/BadExpectedAccess.hpp>

#include "BaseExpectedStorage.hpp"
#include "ExpectedUnion.hpp"

namespace stdx::details {
    template <typename T, typename E>
    class ExpectedStorage : public BaseExpectedStorage<T, E> {
        using Super = BaseExpectedStorage<T, E>;

    public:
        using Super::Super;

        [[nodiscard]] constexpr const T& operator*() const& noexcept {
            return Super::Data.Value;
        }

        [[nodiscard]] constexpr T& operator*() & noexcept {
            return Super::Data.Value;
        }

        [[nodiscard]] constexpr const T&& operator*() const&& noexcept {
            return std::move(Super::Data.Value);
        }

        [[nodiscard]] constexpr T&& operator*() && noexcept {
            return std::move(Super::Data.Value);
        }

        [[nodiscard]] constexpr const T* operator->() const noexcept {
            return std::addressof(**this);
        }

        [[nodiscard]] constexpr T* operator->() noexcept {
            return std::addressof(**this);
        }

    protected:
        template <typename... Ts>
        void ConstructValue(Ts&&... Args) noexcept(NothrowConstructible<T, Ts...>()) {
            ::new (static_cast<void*>(std::addressof(Super::Data.Value))) T(std::forward<Ts>(Args)...);
        }

        template <typename U>
        void AssignValue(U&& Value) noexcept(NothrowAssignable<T&, U>()) {
            Super::Data.Value = std::forward<U>(Value);
        }

        constexpr void DestroyValue() noexcept {
            if constexpr (!TriviallyDestructible<T>()) {
                Super::Data.Value.~T();
            }
        }
    };

    template <typename E>
    class ExpectedStorage<void, E> : public BaseExpectedStorage<void, E> {
        using Super = BaseExpectedStorage<void, E>;

    public:
        using Super::Super;
    };
}