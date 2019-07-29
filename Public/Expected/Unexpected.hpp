#pragma once

#include "Details/UnexpectedTraits.hpp"

namespace stdx
{
	template <typename E>
	class [[nodiscard]] Unexpected final
	{
		static_assert(details::ValidUnexpectedSpecialization<E>());
	public:
		template <
			typename G = E,
			typename std::enable_if_t<details::ConstructibleFromG<E, G>::value, int> = 0
		>
		constexpr explicit Unexpected(G&& Data)
		noexcept(details::NothrowConstructible<E, G>()) : Data(std::forward<G>(Data))
		{
		}

		template <
			typename ...Ts,
			typename std::enable_if_t<details::Constructible<E, Ts...>::value, int> = 0
		>
		constexpr explicit Unexpected(std::in_place_t, Ts&& ...Args)
		noexcept(details::NothrowConstructible<E, Ts...>()) : Data(std::forward<Ts>(Args)...)
		{
		}

		template <
			typename U,
			typename ...Ts,
			typename std::enable_if_t<details::Constructible<E, std::initializer_list<U>&, Ts...>::value, int> = 0
		>
		constexpr explicit Unexpected(std::in_place_t, std::initializer_list<U> List, Ts&& ...Args)
		noexcept(details::NothrowConstructible<E, std::initializer_list<U>&, Ts...>()) :
			Data(List, std::forward<Ts>(Args)...)
		{
		}

		template <
			typename Err,
			typename _Traits = details::CopyConstructibleFromUnexpected<E, Err>,
			typename std::enable_if_t<_Traits::Implicit, int> = 0
		>
		constexpr Unexpected(const Unexpected<Err>& Other) noexcept(_Traits::Nothrow) :
			Data(Other.Value())
		{
		}

		template <
			typename Err,
			typename _Traits = details::CopyConstructibleFromUnexpected<E, Err>,
			typename std::enable_if_t<_Traits::Explicit, int> = 0
		>
		constexpr explicit Unexpected(const Unexpected<Err>& Other) noexcept(_Traits::Nothrow) :
			Data(Other.Value())
		{
		}

		template <
			typename Err,
			typename _Traits = details::MoveConstructibleFromUnexpected<E, Err>,
			typename std::enable_if_t<_Traits::Implicit, int> = 0
		>
		constexpr Unexpected(Unexpected<Err>&& Other) noexcept(_Traits::Nothrow) :
			Data(std::move(Other).Value())
		{
		}

		template <
			typename Err,
			typename _Traits = details::MoveConstructibleFromUnexpected<E, Err>,
			typename std::enable_if_t<_Traits::Explicit, int> = 0
		>
		constexpr explicit Unexpected(Unexpected<Err>&& Other) noexcept(_Traits::Nothrow) :
			Data(std::move(Other).Value())
		{
		}

		[[nodiscard]] constexpr const E& Value() const& noexcept
		{
			return Data;
		}

		[[nodiscard]] constexpr E& Value()& noexcept
		{
			return Data;
		}

		[[nodiscard]] constexpr const E&& Value() const&& noexcept
		{
			return std::move(Data);
		}

		[[nodiscard]] constexpr E&& Value()&& noexcept
		{
			return std::move(Data);
		}

		void Swap(Unexpected& Other) noexcept(details::NothrowSwappable<E>())
		{
			using std::swap;
			swap(Data, Other.Data);
		}

	private:
		E Data;
	};

	template <typename E>
	Unexpected(E) -> Unexpected<E>;

	template <typename E1, typename E2>
	[[nodiscard]] constexpr bool operator==(const Unexpected<E1>& X, const Unexpected<E2>& Y)
	noexcept(noexcept(X.Value() == Y.Value()))
	{
		return X.Value() == Y.Value();
	}

	template <typename E1, typename E2>
	[[nodiscard]] constexpr bool operator!=(const Unexpected<E1>& X, const Unexpected<E2>& Y)
	noexcept(noexcept(X.Value() != Y.Value()))
	{
		return X.Value() != Y.Value();
	}

	template <typename E, typename std::enable_if_t<details::Swappable<E>::value, int> = 0>
	void swap(Unexpected<E>& X, Unexpected<E>& Y) noexcept(noexcept(X.Swap(Y)))
	{
		X.Swap(Y);
	}
}