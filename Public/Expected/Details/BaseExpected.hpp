#pragma once

#include "BaseMoveAssign.hpp"
#include "ExpectedTraits.hpp"

namespace stdx::details
{
	template <typename T, typename E, bool = IsVoid<T>()>
	class BaseExpected : public MoveAssignSelector<T, E>
	{
		using Super = MoveAssignSelector<T, E>;
	protected:
		using Super::Super;

		template <typename ...Ts>
		T& Emplace(Ts&& ...Args) noexcept(noexcept(DoEmplace(std::forward<Ts>(Args)...)))
		{
			return DoEmplace(std::forward<Ts>(Args)...);
		}

		template <typename U, typename ...Ts>
		T& Emplace(std::initializer_list<U> List, Ts&& ...Args) 
		noexcept(noexcept(DoEmplace(List, std::forward<Ts>(Args)...)))
		{
			return DoEmplace(List, std::forward<Ts>(Args)...);
		}
	private:
		template <
			typename ...Ts,
			typename std::enable_if_t<EmplacebleFromTs<T, E, Ts...>::value, int> = 0
		>
		T& DoEmplace(Ts&& ...Args) noexcept(NothrowConstructible<T, Ts...>())
		{
			if (Super::HasValue())
			{
				if constexpr (NothrowConstructible<T, Ts...>())
				{
					Super::DestroyValue();
					Super::ConstructValue(std::forward<Ts>(Args)...);
				}
				else
				{
					Super::AssignValue(T(std::forward<Ts>(Args)...));
				}
			}
			else
			{
				if constexpr (NothrowConstructible<T, Ts...>())
				{
					Super::DestroyUnexpected();
					Super::ConstructValue(std::forward<Ts>(Args)...);
				}
				else if constexpr (NothrowMoveConstructible<T>())
				{
					T Tmp(std::forward<Ts>(Args)...);
					Super::DestroyUnexpected();
					Super::ConstructValue(std::move(Tmp));
				}
				else
				{
					Unexpected<E> Tmp(std::move(Super::error()));
					Super::DestroyUnexpected();
					try
					{
						Super::ConstructValue(std::forward<Ts>(Args)...);
					}
					catch (...)
					{
						Super::Constructunexpected(std::move(Tmp));
						throw;
					}
				}
			}
			Super::bHasValue = true;
			return **this;
		}
	};

	template <typename T, typename E>
	class BaseExpected<T, E, true> : public MoveAssignSelector<void, E>
	{
		using Super = MoveAssignSelector<void, E>;
	protected:
		using Super::Super;

		void Emplace() noexcept
		{
			if (!Super::HasValue())
			{
				Super::DestroyUnexpected();
			}
			Super::bHasValue = true;
		}
	};
}