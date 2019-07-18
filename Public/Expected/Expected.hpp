#pragma once

#include "Details/BaseExpected.hpp"

namespace stdx
{
	template <typename T, typename E>
	class [[nodiscard]] Expected final : public details::BaseExpected<T, E>
	{
		static_assert(details::ValidExpectedSpecialization<T, E>());

		using Super = details::BaseExpected<T, E>;

		static constexpr auto WithValue = std::in_place_index<0>;
		static constexpr auto WithError = std::in_place_index<1>;
	public:
		template <typename U>
		using Rebind = Expected<U, E>;

		using Super::Emplace;

		template <
			typename U = T,
			typename std::enable_if_t<details::VoidOrDefaultConstructible<U>::value, int> = 0
		>
		constexpr Expected()
		noexcept(details::VoidOrNothrowDefaultConstructible<U>()) : Super(WithValue)
		{
		}

		template <
			typename U,
			typename G,
			typename _Traits = details::CopyConstructibleFromExpected<T, E, U, G>,
			typename std::enable_if_t<_Traits::Implicit, int> = 0
		>
		Expected(const Expected<U, G>& Other) noexcept(_Traits::Nothrow)
		{
			if (Other.HasValue())
			{
				if constexpr (!details::IsVoid<T>())
				{
					Super::ConstructValue(*Other);
				}
			}
			else
			{
				Super::ConstructUnexpected(Other.Error());
			}
			Super::bHasValue = Other.HasValue();
		}

		template <
			typename U,
			typename G,
			typename _Traits = details::CopyConstructibleFromExpected<T, E, U, G>,
			typename std::enable_if_t<_Traits::Explicit, int> = 0
		>
		explicit Expected(const Expected<U, G>& Other) noexcept(_Traits::Nothrow)
		{
			if (Other.HasValue())
			{
				if constexpr (!details::IsVoid<T>())
				{
					Super::ConstructValue(*Other);
				}
			}
			else
			{
				Super::ConstructUnexpected(Other.Error());
			}
			Super::bHasValue = Other.HasValue();
		}

		template <
			typename U,
			typename G,
			typename _Traits = details::MoveConstructibleFromExpected<T, E, U, G>,
			typename std::enable_if_t<_Traits::Implicit, int> = 0
		>
		Expected(Expected<U, G>&& Other) noexcept(_Traits::Nothrow)
		{
			if (Other.HasValue())
			{
				if constexpr (!details::IsVoid<T>())
				{
					Super::ConstructValue(std::move(*Other));
				}
			}
			else
			{
				Super::ConstructUnexpected(std::move(Other).Error());
			}
			Super::bHasValue = Other.HasValue();
		}

		template <
			typename U,
			typename G,
			typename _Traits = details::MoveConstructibleFromExpected<T, E, U, G>,
			typename std::enable_if_t<_Traits::Explicit, int> = 0
		>
		explicit Expected(Expected<U, G>&& Other) noexcept(_Traits::Nothrow)
		{
			if (Other.HasValue())
			{
				if constexpr (!details::IsVoid<T>())
				{
					Super::ConstructValue(std::move(*Other));
				}
			}
			else
			{
				Super::ConstructUnexpected(std::move(Other.Error()));
			}
			Super::bHasValue = Other.HasValue();
		}

		template <
			typename U,
			typename _Traits = details::ConstructibleFromU<T, E, U>,
			typename std::enable_if_t<_Traits::Implicit, int> = 0
		>
		constexpr Expected(U&& Value) noexcept(_Traits::Nothrow) : Super(WithValue, std::forward<U>(Value))
		{
		}

		template <
			typename U,
			typename _Traits = details::ConstructibleFromU<T, E, U>,
			typename std::enable_if_t<_Traits::Explicit, int> = 0
		>
		constexpr explicit Expected(U&& Value) noexcept(_Traits::Nothrow) : Super(WithValue, std::forward<U>(Value))
		{
		}

		template <
			typename G = E,
			typename std::enable_if_t<
				details::Constructible<E, const G&>() && details::Convertible<const G&, E>(), int
			> = 0
		>
		constexpr Expected(const Unexpected<G>& Unex)
		noexcept(details::NothrowConstructible<E, const G&>()) : Super(WithError, Unex.Value())
		{
		}

		template <
			typename G = E,
			typename std::enable_if_t<
				details::Constructible<E, const G&>() && !details::Convertible<const G&, E>(), int
			> = 0
		>
		constexpr explicit Expected(const Unexpected<G>& Unex)
		noexcept(details::NothrowConstructible<E, const G&>()) : Super(WithError, Unex.Value())
		{
		}

		template <
			typename G = E,
			typename std::enable_if_t<
				details::Constructible<E, G&&>() && details::Convertible<G&&, E>(), int
			> = 0
		>
		constexpr Expected(Unexpected<G>&& Unex)
		noexcept(details::NothrowConstructible<E, G&&>()) : Super(WithError, std::move(Unex).Value())
		{
		}

		template <
			typename G = E,
			typename std::enable_if_t<
				details::Constructible<E, G&&>() && !details::Convertible<G&&, E>(), int
			> = 0
		>
		constexpr explicit Expected(Unexpected<G>&& Unex)
		noexcept(details::NothrowConstructible<E, G&&>()) : Super(WithError, std::move(Unex).Value())
		{
		}

		template <
			typename ...Ts,
			typename std::enable_if_t<details::ConstructibleInPlace<T, Ts...>::value, int> = 0
		>
		constexpr explicit Expected(std::in_place_t, Ts&& ...Args)
		noexcept(details::NothrowConstructible<T, Ts...>()) : Super(WithValue, std::forward<Ts>(Args)...)
		{
		}

		template <
			typename U,
			typename ...Ts,
			typename std::enable_if_t<
				details::ConstructibleInPlace<T, std::initializer_list<U>&, Ts...>::value,
				int
			> = 0
		>
		constexpr explicit Expected(std::in_place_t, std::initializer_list<U> List, Ts&& ...Args)
		noexcept(details::NothrowConstructible<T, std::initializer_list<U>&, Ts...>()) :
			Super(WithValue, List, std::forward<Ts>(Args)...)
		{
		}

		template <typename ...Ts, typename std::enable_if_t<details::Constructible<E, Ts...>::value, int> = 0>
		constexpr explicit Expected(unexpect_t, Ts&& ...Args)
		noexcept(details::NothrowConstructible<E, Ts...>()) : Super(WithError, std::in_place, std::forward<Ts>(Args)...)
		{
		}

		template <
			typename U,
			typename ...Ts,
			typename std::enable_if_t<details::Constructible<E, std::initializer_list<U>&, Ts...>::value, int> = 0
		>
		constexpr explicit Expected(unexpect_t, std::initializer_list<U> List, Ts&& ...Args)
		noexcept(details::NothrowConstructible<E, std::initializer_list<U>&, Ts...>()) :
			Super(WithError, std::in_place, List, std::forward<Ts>(Args)...)
		{
		}

		template <
			typename G = E,
			typename std::enable_if_t<
				details::And<
					details::Or<details::IsVoid<T>, details::NothrowConstructible<E, const G&>>,
					details::MoveAssignable<E>
				>::value, int
			> = 0
		>
		Expected& operator=(const Unexpected<G>& Unex) noexcept(details::NothrowMoveAssignable<E>())
		{
			if (Super::HasValue())
			{
				if constexpr (!details::IsVoid<T>())
				{
					Super::DestroyValue();
				}
				Super::ConstructUnexpected(Unex.Value());
			}
			else
			{
				Super::AssignUnexpected(Unexpected<E>(Unex.Value()));
			}
			Super::bHasValue = false;
			return *this;
		}

		template <
			typename G = E,
			typename std::enable_if_t<
				details::And<
					details::Or<details::IsVoid<T>, details::NothrowConstructible<E, G&&>>,
					details::MoveAssignable<E>
				>::value, int
			> = 0
		>
		Expected& operator=(Unexpected<G>&& Unex) noexcept(details::NothrowMoveAssignable<E>())
		{
			if (Super::HasValue())
			{
				if constexpr (!details::IsVoid<T>())
				{
					Super::DestroyValue();
				}
				Super::ConstructUnexpected(std::move(Unex).Value());
			}
			else
			{
				Super::AssignUnexpected(Unexpected<E>(std::move(Unex).Value()));
			}
			Super::bHasValue = false;
			return *this;
		}

		template <
			typename U = T,
			typename std::enable_if_t<
				details::And<
					details::Not<details::IsVoid<T>>,
					details::AssignableFromU<T, E, U>
				>::value, int
			> = 0
		>
		Expected& operator=(U&& Value)
		noexcept(details::NothrowConstructible<T, U>() && details::NothrowAssignable<T&, U>())
		{
			if (Super::HasValue())
			{
				Super::AssignValue(std::forward<U>(Value));
			}
			else
			{
				if constexpr (details::NothrowConstructible<T, U>())
				{
					Super::DestroyUnexpected();
					Super::ConstructValue(std::forward<U>(Value));
				}
				else
				{
					Unexpected<E> Tmp(std::move(Super::Error()));
					Super::DestroyUnexpected();
					try
					{
						Super::ConstructValue(std::forward<U>(Value));
					}
					catch (...)
					{
						Super::ConstructUnexpected(std::move(Tmp));
						throw;
					}
				}
			}
			Super::bHasValue = true;
			return *this;
		}

		void Swap(Expected<T, E>& Other)
		noexcept(
		details::And<
			details::VoidOrNothrowMoveConstructible<T>,
			details::VoidOrNothrowSwappable<T>,
			details::NothrowMoveConstructible<E>,
			details::NothrowSwappable<E>
		>())
		{
			using std::swap;

			if (Other.HasValue())
			{
				if (Super::HasValue())
				{
					if constexpr (!details::IsVoid<T>())
					{
						swap(**this, *Other);
					}
				}
				else
				{
					Other.Swap(*this);
				}
			}
			else
			{
				if (Super::HasValue())
				{
					if constexpr (details::IsVoid<T>())
					{
						Super::ConstructUnexpected(std::move(Other).Error());
						Other.DestroyUnexpected();
					}
					else if constexpr (details::NothrowMoveConstructible<T>() && details::NothrowMoveConstructible<E>())
					{
						Unexpected<E> Tmp(std::move(Other).Error());
						Other.DestroyUnexpected();
						Other.ConstructValue(std::move(**this));
						Super::DestroyValue();
						Super::ConstructUnexpected(std::move(Tmp));
					}
					else if constexpr (details::NothrowMoveConstructible<E>())
					{
						Unexpected<E> Tmp(std::move(Other).Error());
						Other.DestroyUnexpected();
						try
						{
							Other.ConstructValue(std::move(**this));
						}
						catch (...)
						{
							Other.ConstructUnexpected(std::move(Tmp));
							throw;
						}
						Super::DestroyValue();
						Super::ConstructUnexpected(std::move(Tmp));
					}
					else if constexpr (details::NothrowMoveConstructible<T>())
					{
						T Tmp(std::move(**this));
						Super::DestroyValue();
						try
						{
							Super::ConstructUnexpected(std::move(Other).Error());
						}
						catch (...)
						{
							Super::ConstructValue(std::move(Tmp));
							throw;
						}
						Other.DestroyUnexpected();
						Other.ConstructValue(std::move(Tmp));
					}
					else
					{
						static_assert(sizeof(T) + sizeof(E) == 0);
					}
					swap(Super::bHasValue, Other.bHasValue);
				}
				else
				{
					swap(Super::Error(), Other.Error());
				}
			}
		}
	};

	template <typename T1, typename E1, typename T2, typename E2>
	constexpr bool operator==(const Expected<T1, E1>& X, const Expected<T2, E2>& Y)
	noexcept(noexcept((*X == *Y), (X.Error() == Y.Error())))
	{
		if (X.HasValue())
		{
			return Y.HasValue() && (*X == *Y);
		}
		return !Y.HasValue() && (X.Error() == Y.Error());
	}

	template <typename T1, typename E1, typename T2, typename E2>
	constexpr bool operator!=(const Expected<T1, E1>& X, const Expected<T2, E2>& Y)
	noexcept(noexcept((*X != *Y), (X.Error() != Y.Error())))
	{
		if (X.HasValue())
		{
			return !Y.HasValue() || (*X != *Y);
		}
		return Y.HasValue() || (X.Error() != Y.Error());
	}

	template <typename T1, typename E1, typename T2>
	constexpr bool operator==(const Expected<T1, E1>& X, const T2& Value) noexcept(noexcept(*X == Value))
	{
		return X.HasValue() && (*X == Value);
	}

	template <typename T1, typename E1, typename T2>
	constexpr bool operator==(const T2& Value, const Expected<T1, E1>& X) noexcept(noexcept(X == Value))
	{
		return X == Value;
	}

	template <typename T1, typename E1, typename T2>
	constexpr bool operator!=(const Expected<T1, E1>& X, const T2& Value) noexcept(noexcept(*X != Value))
	{
		return !X.HasValue() || (*X != Value);
	}

	template <typename T1, typename E1, typename T2>
	constexpr bool operator!=(const T2& Value, const Expected<T1, E1>& X) noexcept(noexcept(X != Value))
	{
		return X != Value;
	}

	template <typename T1, typename E1, typename E2>
	constexpr bool operator==(const Expected<T1, E1>& X, const Unexpected<E2>& Unex)
	noexcept(noexcept(X.Error() == Unex.Value()))
	{
		return !X.HasValue() && (X.Error() == Unex.Value());
	}

	template <typename T1, typename E1, typename E2>
	constexpr bool operator==(const Unexpected<E2>& Unex, const Expected<T1, E1>& X) noexcept(noexcept(X == Unex))
	{
		return X == Unex;
	}

	template <typename T1, typename E1, typename E2>
	constexpr bool operator!=(const Expected<T1, E1>& X, const Unexpected<E2>& Unex)
	noexcept(noexcept(X.Error() != Unex.Value()))
	{
		return X.HasValue() || (X.Error() != Unex.Value());
	}

	template <typename T1, typename E1, typename E2>
	constexpr bool operator!=(const Unexpected<E2>& Unex, const Expected<T1, E1>& X) noexcept(noexcept(X != Unex))
	{
		return X != Unex;
	}

	template <
		typename T1,
		typename E1,
		typename std::enable_if_t<
			details::And<
				details::Or<
					details::IsVoid<T1>,
					details::And<details::MoveConstructible<T1>, details::Swappable<T1>>
				>,
				details::MoveConstructible<E1>,
				details::Swappable<E1>,
				details::Or<details::VoidOrNothrowMoveConstructible<T1>, details::NothrowMoveConstructible<E1>>
			>::value, int
		> = 0
	>
	void swap(Expected<T1, E1>& X, Expected<T1, E1>& Y) noexcept(noexcept(X.Swap(Y)))
	{
		return X.Swap(Y);
	}
}