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
	stdx::Expected<std::vector<int>, std::string> x;
	stdx::Expected<void, std::string> y(stdx::unexpect, {'a', 'b', 'c'}, std::allocator<char>{});
	stdx::Expected<void, std::string_view> z = y;
	stdx::Expected<void, std::string> u(z);
	z = y;
	z = std::move(y);

	stdx::Unexpected<std::string_view> i("abcdef");
	stdx::Unexpected<std::string> k(i);

	y = i;

	const auto m = std::is_literal_type_v<stdx::Unexpected<std::string>>;

	return 0;
}