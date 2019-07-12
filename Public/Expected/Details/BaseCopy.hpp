#pragma once

#include "BaseDestructor.hpp"

namespace stdx::details
{
	template <typename T, typename E, bool Enable>
	struct BaseCopyConstructor : DestructorSelector<T, E>
	{
		using Super = DestructorSelector<T, E>;
		using Super::Super;

		BaseCopyConstructor() = default;

		BaseCopyConstructor(const BaseCopyConstructor&) = delete;

		BaseCopyConstructor(BaseCopyConstructor&&) = default;

		BaseCopyConstructor& operator=(const BaseCopyConstructor&) = default;

		BaseCopyConstructor& operator=(BaseCopyConstructor&&) = default;
	};

	template <typename T, typename E>
	struct BaseCopyConstructor<T, E, true> : DestructorSelector<T, E>
	{
		using Super = DestructorSelector<T, E>;
		using Super::Super;

		BaseCopyConstructor() = default;

		constexpr BaseCopyConstructor(const BaseCopyConstructor& Other)
		noexcept(VoidOrNothrowCopyConstructible<T>() && NothrowCopyConstructible<E>())
		{
			if (Other.HasValue())
			{
				if constexpr (!IsVoid<T>())
				{
					Super::ConstructValue(*Other);
				}
			}
			else
			{
				Super::ConstructUnexpected(Other.Error());
			}
			Super::bHasValue = Other.bHasValue;
		}

		BaseCopyConstructor(BaseCopyConstructor&&) = default;

		BaseCopyConstructor& operator=(const BaseCopyConstructor&) = default;

		BaseCopyConstructor& operator=(BaseCopyConstructor&&) = default;
	};

	template <typename T, typename E>
	using CopySelector =
	Conditional<
		And<VoidOrTriviallyCopyConstructible<T>, TriviallyCopyConstructible<E>>,
		DestructorSelector<T, E>,
		BaseCopyConstructor<T, E, (And<VoidOrCopyConstructible<T>, CopyConstructible<E>>())>
	>;
}

