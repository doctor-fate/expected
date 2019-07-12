#pragma once

#include "BaseCopy.hpp"

namespace stdx::details
{
	template <typename T, typename E, bool Enable>
	struct BaseMoveConstructor : CopySelector<T, E>
	{
		using Super = CopySelector<T, E>;
		using Super::Super;

		BaseMoveConstructor() = default;

		BaseMoveConstructor(const BaseMoveConstructor&) = default;

		BaseMoveConstructor(BaseMoveConstructor&&) = delete;

		BaseMoveConstructor& operator=(const BaseMoveConstructor&) = default;

		BaseMoveConstructor& operator=(BaseMoveConstructor&&) = default;
	};

	template <typename T, typename E>
	struct BaseMoveConstructor<T, E, true> : CopySelector<T, E>
	{
		using Super = CopySelector<T, E>;
		using Super::Super;

		BaseMoveConstructor() = default;

		BaseMoveConstructor(const BaseMoveConstructor&) = default;

		BaseMoveConstructor(BaseMoveConstructor&& Other)
		noexcept(VoidOrNothrowMoveConstructible<T>() && NothrowMoveConstructible<E>())
		{
			if (Other.HasValue())
			{
				if constexpr (!IsVoid<T>())
				{
					Super::ConstructValue(std::move(*Other));
				}
			}
			else
			{
				Super::ConstructUnexpected(std::move(Other).Error());
			}
			Super::bHasValue = Other.bHasValue;
		}

		BaseMoveConstructor& operator=(const BaseMoveConstructor&) = default;

		BaseMoveConstructor& operator=(BaseMoveConstructor&&) = default;
	};

	template <typename T, typename E>
	using MoveSelector =
	Conditional<
		And<VoidOrTriviallyMoveConstructible<T>, TriviallyMoveConstructible<E>>,
		CopySelector<T, E>,
		BaseMoveConstructor<T, E, (And<VoidOrMoveConstructible<T>, MoveConstructible<E>>())>
	>;
}