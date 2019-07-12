#include <Expected/Expected.hpp>

#include <string>
#include <string_view>
#include <vector>

constexpr stdx::Expected<int, std::string_view> f(int x)
{
	if (x <= 0)
	{
		return stdx::Unexpected("x must be greater than zero");
	}
	return 42;
}

// TODO: add normal tests and examples
int main()
{
	{
		using T = stdx::Expected<int, double>;
		static_assert(std::is_nothrow_default_constructible_v<T>);
		static_assert(std::is_trivially_destructible_v<T>);
		static_assert(std::is_trivially_copy_constructible_v<T>);
		static_assert(std::is_trivially_move_constructible_v<T>);
		static_assert(std::is_nothrow_copy_assignable_v<T>);
		static_assert(std::is_nothrow_move_assignable_v<T>);
	}

	{
		using T = stdx::Expected<void, double>;
		static_assert(std::is_nothrow_default_constructible_v<T>);
		static_assert(std::is_trivially_destructible_v<T>);
		static_assert(std::is_trivially_copy_constructible_v<T>);
		static_assert(std::is_trivially_move_constructible_v<T>);
		static_assert(std::is_nothrow_copy_assignable_v<T>);
		static_assert(std::is_nothrow_move_assignable_v<T>);
	}

	{
		using T = stdx::Expected<int, std::string>;
		static_assert(std::is_nothrow_default_constructible_v<T>);
		static_assert(!std::is_trivially_destructible_v<T>);
		static_assert(!std::is_trivially_copy_constructible_v<T>);
		static_assert(!std::is_trivially_move_constructible_v<T>);
		static_assert(std::is_copy_constructible_v<T>);
		static_assert(std::is_nothrow_move_constructible_v<T>);
		static_assert(std::is_copy_assignable_v<T>);
		static_assert(std::is_nothrow_move_assignable_v<T>);
	}

	{
		using T = stdx::Expected<std::unique_ptr<int>, std::string>;
		static_assert(std::is_nothrow_default_constructible_v<T>);
		static_assert(!std::is_trivially_destructible_v<T>);
		static_assert(!std::is_trivially_copy_constructible_v<T>);
		static_assert(!std::is_trivially_move_constructible_v<T>);
		static_assert(!std::is_copy_constructible_v<T>);
		static_assert(std::is_nothrow_move_constructible_v<T>);
		static_assert(!std::is_copy_assignable_v<T>);
		static_assert(std::is_nothrow_move_assignable_v<T>);
	}

	{
		struct NonSuch
		{
			NonSuch(NonSuch&&) = delete;

			NonSuch& operator=(NonSuch&&) = delete;
		};

		using T = stdx::Expected<NonSuch, int>;
		static_assert(!std::is_default_constructible_v<T>);
		static_assert(std::is_trivially_destructible_v<T>);
		static_assert(!std::is_copy_constructible_v<T>);
		static_assert(!std::is_move_constructible_v<T>);
		static_assert(!std::is_copy_assignable_v<T>);
		static_assert(!std::is_move_assignable_v<T>);
	}

	return 0;
}