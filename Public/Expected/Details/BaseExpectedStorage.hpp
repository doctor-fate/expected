#pragma once

#include "ExpectedUnion.hpp"

#include <new>

namespace stdx::details
{
	template <typename T, typename E>
	class BaseExpectedStorage
	{
	public:
		using ValueType      = T;
		using ErrorType      = E;
		using UnexpectedType = Unexpected<E>;

		constexpr bool HasValue() const noexcept
		{
			return bHasValue;
		}

		constexpr explicit operator bool() const noexcept
		{
			return HasValue();
		}

		constexpr const E& Error() const& noexcept
		{
			return Data.Unex.Value();
		}

		constexpr E& Error()& noexcept
		{
			return Data.Unex.Value();
		}

		constexpr const E&& Error() const&& noexcept
		{
			return std::move(Data.Unex).Value();
		}

		constexpr E&& Error()&& noexcept
		{
			return std::move(Data.Unex).Value();
		}

	protected:
		constexpr BaseExpectedStorage() noexcept : Data(valueless), bHasValue(true)
		{
		}

		template <typename ...Ts>
		explicit constexpr BaseExpectedStorage(std::in_place_index_t<0>, Ts&& ...Args)
		noexcept(NothrowConstructible<T, Ts...>()) :
			Data(std::in_place_index<0> , std::forward<Ts>(Args)...), bHasValue(true)
		{
		}

		template <typename ...Ts>
		explicit constexpr BaseExpectedStorage(std::in_place_index_t<1>, Ts&& ...Args)
		noexcept(NothrowConstructible<E, Ts...>()) :
			Data(std::in_place_index<1> , std::forward<Ts>(Args)...), bHasValue(false)
		{
		}

		template <typename ...Ts>
		void ConstructUnexpected(Ts&& ...Args) noexcept(NothrowConstructible<E, Ts...>())
		{
			::new (static_cast<void*>(std::addressof(Data.Unex))) Unexpected<E>(std::forward<Ts>(Args)...);
		}

		void AssignUnexpected(Unexpected <E>&& e) noexcept(NothrowMoveAssignable<E>())
		{
			Data.Unex = std::move(e);
		}

		void DestroyUnexpected() noexcept
		{
			Data.Unex.~Unexpected<E>();
		}

		ExpectedUnion<T, E> Data;
		bool bHasValue;
	};
}