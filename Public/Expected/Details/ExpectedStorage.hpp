#include <new>
#include <memory>
#include <utility>

#include <Expected/BadExpectedAccess.hpp>

#include "BaseExpectedStorage.hpp"
#include "ExpectedUnion.hpp"

namespace stdx::details
{
	template <typename T, typename E>
	class ExpectedStorage : public BaseExpectedStorage<T, E>
	{
		using Super = BaseExpectedStorage<T, E>;
	public:
		constexpr const T& operator*() const& noexcept
		{
			return Super::Data.Value;
		}

		constexpr T& operator*()& noexcept
		{
			return Super::Data.Value;
		}

		constexpr const T&& operator*() const&& noexcept
		{
			return std::move(Super::Data.Value);
		}

		constexpr T&& operator*()&& noexcept
		{
			return std::move(Super::Data.Value);
		}

		constexpr const T* operator->() const noexcept
		{
			return std::addressof(**this);
		}

		constexpr T* operator->() noexcept
		{
			return std::addressof(**this);
		}

		constexpr const T& Value() const&
		{
			if (!Super::HasValue())
			{
				throw BadExpectedAccess(Super::Error());
			}
			return **this;
		}

		constexpr T& Value()&
		{
			if (!Super::HasValue())
			{
				throw BadExpectedAccess(Super::Error());
			}
			return **this;
		}

		constexpr const T&& Value() const&&
		{
			if (!Super::HasValue())
			{
				throw BadExpectedAccess(Super::Error());
			}
			return std::move(**this);
		}

		constexpr T&& Value()&&
		{
			if (!Super::HasValue())
			{
				throw BadExpectedAccess(Super::Error());
			}
			return std::move(**this);
		}

		template <typename U>
		constexpr T ValueOr(U&& Default) const&
		{
			return Super::HasValue() ? **this : static_cast<T>(std::forward<U>(Default));
		}

		template <typename U>
		constexpr T ValueOr(U&& Default)&&
		{
			return Super::HasValue() ? std::move(**this) : static_cast<T>(std::forward<U>(Default));
		}

	protected:
		using Super::Super;

		template <typename ...Ts>
		void ConstructValue(Ts&& ...Args) noexcept(NothrowConstructible<T, Ts...>())
		{
			::new(static_cast<void*>(std::addressof(Super::Data.Value))) T(std::forward<Ts>(Args)...);
		}

		template <typename U>
		void AssignValue(U&& Value) noexcept(NothrowAssignable<T&, U>())
		{
			Super::Data.Value = std::forward<U>(Value);
		}


		void DestroyValue() noexcept
		{
			Super::Data.Value.~T();
		}
	};

	template <typename E>
	class ExpectedStorage<void, E> : public BaseExpectedStorage<void, E>
	{
		using Super = BaseExpectedStorage<void, E>;
	public:
		constexpr void Value() const
		{
			if (!Super::HasValue())
			{
				throw BadExpectedAccess<void>();
			}
		}

	protected:
		using Super::Super;
	};
}