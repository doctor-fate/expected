#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include <Expected/Unexpected.hpp>

namespace stdx::tests
{
	TEST(Unexpected, Constructors)
	{
		{
			Unexpected<std::string> Unex("hello, world!!!");
			ASSERT_EQ(Unex.Value(), "hello, world!!!");
		}

		{
			Unexpected<std::vector<int>> Unex(42);
			ASSERT_EQ(size(Unex.Value()), 42);
		}

		{
			Unexpected<std::string> Unex(std::in_place, 4, 'a');
			ASSERT_EQ(Unex.Value(), "aaaa");
		}

		{
			Unexpected<std::string> Unex(std::in_place, {'a', 'b', 'c', 'd'}, std::allocator<int>{ });
			ASSERT_EQ(Unex.Value(), "abcd");
		}

		{
			Unexpected Unex1(new int(42));
			Unexpected<std::unique_ptr<int>> Unex2(Unex1);
			ASSERT_EQ(*Unex2.Value(), 42);
		}

		{
			Unexpected Unex1("hello");
			Unexpected<std::string> Unex2 = Unex1;
			ASSERT_EQ(Unex2.Value(), "hello");
		}

		{
			Unexpected Unex1(new int(42));
			Unexpected<std::unique_ptr<int>> Unex2(std::move(Unex1));
			ASSERT_EQ(*Unex2.Value(), 42);
		}

		{
			Unexpected Unex1("hello");
			Unexpected<std::string> Unex2 = std::move(Unex1);
			ASSERT_EQ(Unex2.Value(), "hello");
		}
	}

	TEST(Unexpected, Accessors)
	{
		{
			Unexpected<std::string> Unex1("hello");
			ASSERT_EQ(Unex1.Value(), "hello");
			ASSERT_EQ(std::move(Unex1).Value(), "hello");
			const Unexpected<std::string> Unex2("hello");
			ASSERT_EQ(Unex2.Value(), "hello");
			ASSERT_EQ(std::move(Unex2).Value(), "hello");
		}
	}

	TEST(Unexpected, Swap)
	{
		using std::swap;
		{
			Unexpected<std::string> Unex1("hello"), Unex2("world");
			swap(Unex1, Unex2);
			ASSERT_EQ(Unex1.Value(), "world");
			ASSERT_EQ(Unex2.Value(), "hello");
		}
	}

	TEST(Unexpected, EqCompare)
	{
		{
			Unexpected<std::string> Unex1("hello"), Unex2("world");
			ASSERT_EQ(Unex1, Unex1);
			ASSERT_NE(Unex1, Unex2);
		}
	}
}