#include <Expected/Expected.hpp>

#include <gtest/gtest.h>

namespace stdx::tests
{
	TEST(Expected, Constructors)
	{
		{
			Expected<int, double> e;
			ASSERT_TRUE(e.HasValue());
			ASSERT_EQ(*e, 0);
		}

		{
			Expected<int, double> e(42);
			ASSERT_TRUE(e.HasValue());
			ASSERT_EQ(*e, 42);
		}

		{
			Expected<int, double> e = 42;
			ASSERT_TRUE(e.HasValue());
			ASSERT_EQ(*e, 42);
		}

		{
			Expected<int, double> e(Unexpected(42.0));
			ASSERT_FALSE(e.HasValue());
			ASSERT_EQ(e.Error(), 42.0);
		}

		{
			Unexpected Unex(42.0f);
			Expected<int, double> e = Unex;
			ASSERT_FALSE(e.HasValue());
			ASSERT_EQ(e.Error(), 42.0);
		}

		{
			Expected<int, double> e1(42), e2(e1);
			ASSERT_EQ(e1, e2);
		}

		{
			Expected<int, double> e1(Unexpected(54.0));
			Expected<int, double> e2 = std::move(e1);
			ASSERT_FALSE(e2.HasValue());
			ASSERT_EQ(e2.Error(), 54.0);
		}

		{
			Expected<double, float> e1(Unexpected(54.0));
			Expected<int, double> e2(e1);
			ASSERT_EQ(e1, e2);
		}

		{
			Expected<std::vector<int>, std::string> e(std::in_place, {1, 2, 3});
			ASSERT_EQ(e.Value(), (std::vector<int>{1, 2, 3}));
		}

		{
			Expected<std::vector<int>, std::string> e(unexpect, {'a', 'b', 'c', 'd'});
			ASSERT_EQ(e.Error(), "abcd");
		}

		{
			Expected<std::vector<int>, std::string> e(3);
			ASSERT_EQ(size(e.Value()), 3);
		}

		{
			Expected<std::vector<int>, std::string> e(std::in_place, 3);
			ASSERT_EQ(size(e.Value()), 3);
		}

		{
			Expected<std::vector<int>, std::string> e(unexpect, 3, 'a');
			ASSERT_EQ(e.Error(), "aaa");
		}

		{
			Expected<std::unique_ptr<int>, std::string> e1 = std::make_unique<int>(42);
			const auto e2 = std::move(e1);
			ASSERT_EQ(**e2, 42);
		}

		{
			Expected<void, std::string> e;
			ASSERT_TRUE(e.HasValue());
		}

		{
			Expected<void, std::string> e = Unexpected("abcdef");
			ASSERT_EQ(e.Error(), "abcdef");
		}
	}
}