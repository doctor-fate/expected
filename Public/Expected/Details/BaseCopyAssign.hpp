#pragma once

#include "BaseMove.hpp"

namespace stdx::details
{
	template <typename T, typename E, bool Enable>
	struct BaseCopyAssignment : MoveSelector<T, E>
	{
		using Super = MoveSelector<T, E>;
		using Super::Super;

		BaseCopyAssignment() = default;

		BaseCopyAssignment(const BaseCopyAssignment&) = default;

		BaseCopyAssignment(BaseCopyAssignment&&) = default;

		BaseCopyAssignment& operator=(const BaseCopyAssignment&) = delete;

		BaseCopyAssignment& operator=(BaseCopyAssignment&&) = default;
	};

	template <typename T, typename E>
	struct BaseCopyAssignment<T, E, true> : MoveSelector<T, E>
	{
		using Super = MoveSelector<T, E>;
		using Super::Super;

		BaseCopyAssignment() = default;

		BaseCopyAssignment(const BaseCopyAssignment&) = default;

		BaseCopyAssignment(BaseCopyAssignment&&) = default;

		BaseCopyAssignment& operator=(const BaseCopyAssignment& Other)
		noexcept(
		And<
			VoidOrNothrowCopyConstructible<T>,
			NothrowCopyConstructible<E>,
			VoidOrNothrowCopyAssignable<T>,
			NothrowMoveAssignable<E>>()
		)
		{
			if (Other.HasValue())
			{
				if (Super::HasValue())
				{
					if constexpr (!IsVoid<T>())
					{
						Super::AssignValue(*Other);
					}
				}
				else
				{
					if constexpr (IsVoid<T>())
					{
						Super::DestroyUnexpected();
					}
					else if constexpr (NothrowCopyConstructible<T>())
					{
						Super::DestroyUnexpected();
						Super::ConstructValue(*Other);
					}
					else if constexpr (NothrowMoveConstructible<T>())
					{
						T Tmp(*Other);
						Super::DestroyUnexpected();
						Super::ConstructValue(std::move(Tmp));
					}
					else
					{
						Unexpected<E> Tmp(std::move(Super::Error()));
						Super::DestroyUnexpected();
						try
						{
							Super::ConstructValue(*Other);
						}
						catch (...)
						{
							Super::ConstructUnexpected(std::move(Tmp));
							throw;
						}
					}
				}
			}
			else
			{
				if (Super::HasValue())
				{
					if constexpr (IsVoid<T>())
					{
						Super::ConstructUnexpected(Other.Error());
					}
					else if constexpr (NothrowCopyConstructible<E>())
					{
						Super::DestroyValue();
						Super::ConstructUnexpected(Other.Error());
					}
					else if constexpr (NothrowMoveConstructible<E>())
					{
						Unexpected<E> Tmp(Other.Error());
						Super::DestroyValue();
						Super::ConstructUnexpected(std::move(Tmp));
					}
					else
					{
						T Tmp(std::move(**this));
						Super::DestroyValue();
						try
						{
							Super::ConstructUnexpected(Other.Error());
						}
						catch (...)
						{
							Super::ConstructValue(std::move(Tmp));
							throw;
						}
					}
				}
				else
				{
					Super::AssignUnexpected(Unexpected<E>(Other.Error()));
				}
			}
			Super::bHasValue = Other.bHasValue;
			return *this;
		}

		BaseCopyAssignment& operator=(BaseCopyAssignment&&) = default;
	};

	template <typename T, typename E>
	using CopyAssignSelector =
	BaseCopyAssignment<
		T, E,
		(And<
			Or<
				IsVoid<T>,
				And<
					CopyAssignable<T>,
					CopyConstructible<T>,
					Or<NothrowMoveConstructible<T>, NothrowMoveConstructible<E>>
				>
			>,
			MoveAssignable<E>,
			CopyConstructible<E>
		>())
	>;
}

