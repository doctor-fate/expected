#pragma once

#include <exception>
#include <utility>

namespace stdx
{
	template <typename E>
	class BadExpectedAccess;

	template <>
	class BadExpectedAccess<void> : public std::exception
	{
	public:
		explicit BadExpectedAccess() = default;
	};

	template <typename E>
	class BadExpectedAccess : public BadExpectedAccess<void>
	{
	public:
		explicit BadExpectedAccess(E Value) noexcept : Value(std::move(Value))
		{
		}

		const char* what() const noexcept override
		{
			return "bad expected access";
		}

		const E& Error() const noexcept
		{
			return Value;
		}

	private:
		E Value;
	};
}