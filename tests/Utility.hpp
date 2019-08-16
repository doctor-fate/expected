#pragma once

#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace stdx::tests
{
    template <typename T, typename U, typename = std::void_t<>>
    struct IsExplicitlyConstructible : std::false_type
    {
    };

    template <typename T, typename U>
    struct IsExplicitlyConstructible<T, U, std::void_t<decltype(T(std::declval<U>()))>> : std::true_type
    {
    };

    template <typename T>
    struct IsImplicitlyConstructibleHelper
    {
        static constexpr std::true_type __Dummy(const T&);

        static constexpr std::false_type __Dummy(...);
    };

    template <typename T, typename U>
    struct IsImplicitlyConstructible : decltype(IsImplicitlyConstructibleHelper<T>::__Dummy(std::declval<U>()))
    {
    };

    struct NonDefaultConstructible
    {
        NonDefaultConstructible() = delete;
    };

    struct ThrowableCopyConstructible
    {
        explicit ThrowableCopyConstructible(std::string Value, bool bThrow) noexcept : bThrow(bThrow), Value(std::move(Value))
        {
        }

        ThrowableCopyConstructible(const ThrowableCopyConstructible& Other) : bThrow(Other.bThrow)
        {
            if (bThrow)
            {
                throw std::logic_error{"oops"};
            }
            else
            {
                Value = Other.Value;
            }
        }

        ThrowableCopyConstructible(ThrowableCopyConstructible&& Other) = delete;

        ThrowableCopyConstructible& operator=(const ThrowableCopyConstructible&) = default;

        ThrowableCopyConstructible& operator=(ThrowableCopyConstructible&&) = default;

        bool bThrow = false;
        std::string Value;
    };

    bool operator==(const ThrowableCopyConstructible& A, const ThrowableCopyConstructible& B) noexcept
    {
        return A.bThrow == B.bThrow && A.Value == B.Value;
    }

    bool operator!=(const ThrowableCopyConstructible& A, const ThrowableCopyConstructible& B) noexcept
    {
        return A.bThrow != B.bThrow || A.Value != B.Value;
    }

    struct ThrowableMoveConstructible
    {
        explicit ThrowableMoveConstructible(std::string Value, bool bThrow) noexcept : bThrow(bThrow), Value(std::move(Value))
        {
        }

        ThrowableMoveConstructible(const ThrowableMoveConstructible&) = default;

        ThrowableMoveConstructible(ThrowableMoveConstructible&& Other) : bThrow(Other.bThrow)
        {
            if (bThrow)
            {
                throw std::logic_error{"oops"};
            }
            else
            {
                Value = std::move(Other.Value);
            }
        }

        ThrowableMoveConstructible& operator=(const ThrowableMoveConstructible&) = default;

        ThrowableMoveConstructible& operator=(ThrowableMoveConstructible&&) = default;

        bool bThrow = false;
        std::string Value;
    };

    bool operator==(const ThrowableMoveConstructible& A, const ThrowableMoveConstructible& B) noexcept
    {
        return A.bThrow == B.bThrow && A.Value == B.Value;
    }

    bool operator!=(const ThrowableMoveConstructible& A, const ThrowableMoveConstructible& B) noexcept
    {
        return A.bThrow != B.bThrow || A.Value != B.Value;
    }

    struct ThrowableEmplaceConstructible
    {
        ThrowableEmplaceConstructible(int X, double, const std::string&)
        {
            if (X == 42)
            {
                throw std::logic_error{"oops"};
            }
        }

        ThrowableEmplaceConstructible(const ThrowableEmplaceConstructible&) = default;

        ThrowableEmplaceConstructible(ThrowableEmplaceConstructible&& Other)
        {
        }

        ThrowableEmplaceConstructible& operator=(const ThrowableEmplaceConstructible&) = default;

        ThrowableEmplaceConstructible& operator=(ThrowableEmplaceConstructible&&) = default;
    };
}