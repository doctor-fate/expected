#include <Expected/Expected.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace stdx::tests
{
	template <typename T, typename U, typename = std::void_t<>>
	struct IsExplicitlyConstructible : std::false_type
	{
	};

	template <typename T, typename U>
	struct IsExplicitlyConstructible<T, U, std::void_t<decltype(T(std::declval<U>()))>> : std::true_type
	{
	};

	template <typename T>
	struct IsImplicitlyConstructibleHelper
	{
		static constexpr std::true_type __Dummy(const T&);

		static constexpr std::false_type __Dummy(...);
	};

	template <typename T, typename U>
	struct IsImplicitlyConstructible : decltype(IsImplicitlyConstructibleHelper<T>::__Dummy(std::declval<U>()))
	{
	};


	TEST(Expected, Constructor_Default)
	{
		{
			Expected<int, double> Ex;
			ASSERT_EQ(*Ex, 0);
		}

		{
			Expected<std::vector<int>, std::string> Ex;
			ASSERT_TRUE(empty(*Ex));
		}

		{
			struct S
			{
				S() = delete;
			};

			using T = Expected<S, double>;
			static_assert(!std::is_default_constructible_v<T>);
		}
	}

	TEST(Expected, Constructor_FromU)
	{
		{
			Expected<int, double> Ex1(42);
			Expected<int, double> Ex2 = 42;
			ASSERT_EQ(*Ex1, 42);
			ASSERT_EQ(*Ex2, 42);
		}

		{
			using T = Expected<std::vector<int>, std::string>;
			static_assert(!IsImplicitlyConstructible<T, int>());
			T Ex(3);
			ASSERT_EQ(size(*Ex), 3);
		}

		{
			struct S
			{
				explicit S(int) = delete;

				constexpr S(long Value) noexcept : Value(Value)
				{
				}

				long Value;
			};

			using T = Expected<S, double>;
			static_assert(!IsExplicitlyConstructible<T, int>());
			static_assert(!IsImplicitlyConstructible<T, int>());
			Expected<S, double> Ex1(42L);
			Expected<S, double> Ex2 = 42L;
			ASSERT_EQ(Ex1->Value, 42);
			ASSERT_EQ(Ex2->Value, 42);
		}
	}

	TEST(Expected, Constructor_FromExpected)
	{
		{
			Expected<int, double> Ex1(42);
			Expected<long, float> Ex2(Ex1);
			ASSERT_EQ(*Ex1, 42);
			ASSERT_EQ(*Ex2, 42);
		}

		{
			Expected<int, double> Ex1(42);
			Expected<long, float> Ex2 = Ex1;
			ASSERT_EQ(*Ex1, 42);
			ASSERT_EQ(*Ex2, 42);
		}

		{
			using T1 = Expected<int, std::string>;
			using T2 = Expected<std::vector<int>, std::string_view>;

			T1 Ex1(Unexpected("unexpected"));
			T2 Ex2(std::move(Ex1));
			ASSERT_EQ(Ex2.Error(), "unexpected");

			static_assert(!IsImplicitlyConstructible<T1, T2&&>());
			static_assert(!IsExplicitlyConstructible<T1, T2&&>());
			static_assert(!IsImplicitlyConstructible<T2, T1&&>());
		}
	}

	TEST(Expected, Constructor_FromUnexpected)
	{
		{
			Unexpected Unex("unexpected");
			Expected<int, std::string> Ex = Unex;
			ASSERT_EQ(Ex.Error(), "unexpected");
		}

		{
			Unexpected Unex(3);
			Expected<int, std::vector<int>> Ex(Unex);
			ASSERT_EQ(size(Ex.Error()), 3);
		}

		{
			Expected<int, std::string> Ex = Unexpected("unexpected");
			ASSERT_EQ(Ex.Error(), "unexpected");
		}

		{
			Expected<int, std::vector<int>> Ex(Unexpected(3));
			ASSERT_EQ(size(Ex.Error()), 3);
		}
	}

	TEST(Expected, Constructor_InPlace)
	{
		{
			struct S
			{
				S(int X1, std::string X2, double X3) noexcept : X1(X1), X2(std::move(X2)), X3(X3)
				{
				}

				int X1;
				std::string X2;
				double X3;
			};

			Expected<S, std::string> Ex(std::in_place, 1, "hello", 4.0);
			ASSERT_EQ(Ex->X1, 1);
			ASSERT_EQ(Ex->X2, "hello");
			ASSERT_EQ(Ex->X3, 4.0);
		}

		{
			Expected<std::vector<int>, std::string> Ex(std::in_place, {1, 2, 3, 4}, std::allocator<int>{});
			ASSERT_EQ(*Ex, (std::vector<int>{1, 2, 3, 4}));
		}
	}

	TEST(Expected, Constructor_Unexpect)
	{
		{
			struct S
			{
				S(int X1, std::string X2, double X3) noexcept : X1(X1), X2(std::move(X2)), X3(X3)
				{
				}

				int X1;
				std::string X2;
				double X3;
			};

			Expected<std::string, S> Ex(unexpect, 1, "hello", 4.0);
			ASSERT_EQ(Ex.Error().X1, 1);
			ASSERT_EQ(Ex.Error().X2, "hello");
			ASSERT_EQ(Ex.Error().X3, 4.0);
		}

		{
			Expected<std::string, std::vector<int>> Ex(unexpect, {1, 2, 3, 4}, std::allocator<int>{});
			ASSERT_EQ(Ex.Error(), (std::vector<int>{1, 2, 3, 4}));
		}
	}

	TEST(Expected, Constructor_Copy)
	{
		{
			Expected<int, double> Ex1(42);
			Expected<int, double> Ex2 = Ex1;
			ASSERT_EQ(Ex1, Ex2);
		}

		{
			Expected<std::vector<int>, std::string> Ex1(42);
			Expected<std::vector<int>, std::string> Ex2 = Ex1;
			ASSERT_EQ(Ex1, Ex2);
		}

		{
			Expected<std::vector<int>, std::string> Ex1(Unexpected("unexpected"));
			Expected<std::vector<int>, std::string> Ex2 = Ex1;
			ASSERT_EQ(Ex1, Ex2);
		}

		{
			using T = Expected<std::unique_ptr<int>, std::string>;
			static_assert(!std::is_copy_constructible_v<T>);
		}
	}

	TEST(Expected, Constructor_Move)
	{
		{
			Expected<int, double> Ex1(42);
			Expected<int, double> Ex2 = std::move(Ex1);
			ASSERT_EQ(*Ex2, 42);
		}

		{
			Expected<std::vector<int>, std::string> Ex1(42);
			Expected<std::vector<int>, std::string> Ex2 = std::move(Ex1);
			ASSERT_EQ(size(*Ex2), 42);
		}

		{
			Expected<std::vector<int>, std::string> Ex1(Unexpected("unexpected"));
			Expected<std::vector<int>, std::string> Ex2 = std::move(Ex1);
			ASSERT_EQ(Ex2.Error(), "unexpected");
		}

		{
			Expected<std::unique_ptr<int>, std::string> Ex1 = std::make_unique<int>(42);
			Expected<std::unique_ptr<int>, std::string> Ex2 = std::move(Ex1);
			ASSERT_EQ(*Ex1, nullptr);
			ASSERT_EQ(**Ex2, 42);
		}
	}

	TEST(Expected, Assign_Unexpected)
	{
		{
			Unexpected Unex("hello");
			Expected<std::vector<int>, std::string> Ex(42);
			Ex = Unex;
			ASSERT_EQ(Ex.Error(), "hello");
		}

		{
			Unexpected Unex("world");
			Expected<std::vector<int>, std::string> Ex(Unexpected("hello"));
			Ex = Unex;
			ASSERT_EQ(Ex.Error(), "world");
		}

		{
			Expected<std::vector<int>, std::string> Ex(42);
			Ex = Unexpected("hello");
			ASSERT_EQ(Ex.Error(), "hello");
		}

		{
			Expected<std::vector<int>, std::string> Ex(Unexpected("hello"));
			Ex = Unexpected("world");
			ASSERT_EQ(Ex.Error(), "world");
		}
	}

	TEST(Expected, Assign_U)
	{
		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex(42);
			Ex = Vector;
			ASSERT_EQ(*Ex, Vector);
		}

		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex(Unexpected("hello"));
			Ex = Vector;
			ASSERT_EQ(*Ex, Vector);
		}

		{
			Expected<std::string, std::vector<int>> Ex("hello");
			Ex = "world";
			ASSERT_EQ(*Ex, "world");
		}

		{
			Expected<std::string, std::vector<int>> Ex(Unexpected(54));
			Ex = "world"sv;
			ASSERT_EQ(*Ex, "world");
		}

		{
			Expected<std::string, std::vector<int>> Ex(Unexpected(54));
			Ex = "world"s;
			ASSERT_EQ(*Ex, "world");
		}

		{
			struct S
			{
				S() = default;

				S(int Value)
				{
					throw std::exception{};
				}
			};

			Expected<S, std::string> Ex(Unexpected("long long long string"));
			ASSERT_THROW(Ex = 42, std::exception);
			ASSERT_EQ(Ex.Error(), "long long long string");
		}
	}

	TEST(Expected, Assign_Copy)
	{
		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex1(Vector);
			Expected<std::vector<int>, std::string> Ex2(54);
			Ex2 = Ex1;
			ASSERT_EQ(Ex2, Ex1);
		}

		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex1(Vector);
			Expected<std::vector<int>, std::string> Ex2(Unexpected("hello"));
			Ex2 = Ex1;
			ASSERT_EQ(Ex2, Ex1);
		}

		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex1(Unexpected("hello"));
			Expected<std::vector<int>, std::string> Ex2(Vector);
			Ex2 = Ex1;
			ASSERT_EQ(Ex2, Ex1);
		}

		{
			Expected<std::vector<int>, std::string> Ex1(Unexpected("hello"));
			Expected<std::vector<int>, std::string> Ex2(Unexpected("world"));
			Ex2 = Ex1;
			ASSERT_EQ(Ex2, Ex1);
		}

		struct S
		{
			explicit S(std::string Value) noexcept : Value(std::move(Value))
			{
			}

			S(const S& Other) : Value(Other.Value)
			{
				throw std::exception{};
			}

			S(S&& Other) : Value(std::move(Other.Value))
			{
			}

			S& operator=(const S&) = default;

			S& operator=(S&&) = default;

			std::string Value;
		};

		{
			Expected<S, std::string> Ex1("hello");
			Expected<S, std::string> Ex2(Unexpected("world"));
			ASSERT_THROW(Ex2 = Ex1, std::exception);
			ASSERT_EQ(Ex2.Error(), "world");
		}

		{
			Expected<std::string, S> Ex1(Unexpected("hello"));
			Expected<std::string, S> Ex2("world");
			ASSERT_THROW(Ex2 = Ex1, std::exception);
			ASSERT_EQ(*Ex2, "world");
		}
	}

	TEST(Expected, Assign_Move)
	{
		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex1(Vector);
			Expected<std::vector<int>, std::string> Ex2(54);
			Ex2 = std::move(Ex1);
			ASSERT_EQ(*Ex2, Vector);
		}

		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex1(Vector);
			Expected<std::vector<int>, std::string> Ex2(Unexpected("hello"));
			Ex2 = std::move(Ex1);
			ASSERT_EQ(*Ex2, Vector);
		}

		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex1(Unexpected("hello"));
			Expected<std::vector<int>, std::string> Ex2(Vector);
			Ex2 = Ex1;
			ASSERT_EQ(Ex2.Error(), "hello");
		}

		{
			Expected<std::vector<int>, std::string> Ex1(Unexpected("hello"));
			Expected<std::vector<int>, std::string> Ex2(Unexpected("world"));
			Ex2 = Ex1;
			ASSERT_EQ(Ex2.Error(), "hello");
		}

		struct S
		{
			explicit S(std::string Value) noexcept : Value(std::move(Value))
			{
			}

			S(const S& Other) = default;

			S(S&& Other) : Value(std::move(Other.Value))
			{
				throw std::exception{};
			}

			S& operator=(const S&) = default;

			S& operator=(S&&) = default;

			std::string Value;
		};

		{
			Expected<S, std::string> Ex1("hello");
			Expected<S, std::string> Ex2(Unexpected("world"));
			ASSERT_THROW(Ex2 = std::move(Ex1), std::exception);
			ASSERT_EQ(Ex2.Error(), "world");
		}

		{
			Expected<std::string, S> Ex1(Unexpected("hello"));
			Expected<std::string, S> Ex2("world");
			ASSERT_THROW(Ex2 = std::move(Ex1), std::exception);
			ASSERT_EQ(*Ex2, "world");
		}
	}

	TEST(Expected, Swap)
	{
		using std::swap;

		{
			std::vector<int> Vector1{1, 2, 3, 4}, Vector2{5, 6, 7, 8};
			Expected<std::vector<int>, std::string> Ex1(Vector1);
			Expected<std::vector<int>, std::string> Ex2(Vector2);
			swap(Ex2, Ex1);
			ASSERT_EQ(*Ex1, Vector2);
			ASSERT_EQ(*Ex2, Vector1);
		}

		{
			std::vector<int> Vector1{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex1(Vector1);
			Expected<std::vector<int>, std::string> Ex2(Unexpected("hello"));
			swap(Ex2, Ex1);
			ASSERT_EQ(Ex1.Error(), "hello");
			ASSERT_EQ(*Ex2, Vector1);
		}
	}

	TEST(Expected, Emplace)
	{
		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex(42);
			Ex.Emplace(Vector);
			ASSERT_EQ(*Ex, Vector);
		}

		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex(Unexpected("hello"));
			Ex.Emplace(Vector);
			ASSERT_EQ(*Ex, Vector);
		}

		{
			std::vector<int> Vector{1, 2, 3, 4};
			Expected<std::vector<int>, std::string> Ex(Unexpected("hello"));
			Ex.Emplace({1, 2, 3, 4}, std::allocator<int>{});
			ASSERT_EQ(*Ex, Vector);
		}

		{
			Expected<std::string, std::vector<int>> Ex("hello");
			Ex.Emplace("world");
			ASSERT_EQ(*Ex, "world");
		}

		{
			Expected<std::string, std::vector<int>> Ex(Unexpected(54));
			Ex.Emplace("world"sv);
			ASSERT_EQ(*Ex, "world");
		}

		{
			Expected<std::string, std::vector<int>> Ex(Unexpected(54));
			Ex.Emplace("world"s);
			ASSERT_EQ(*Ex, "world");
		}

		struct S
		{
			S() = default;

			S(S&&)
			{
			}

			S(int Value)
			{
				throw std::exception{};
			}

			S& operator=(S&&) = default;
		};

		{
			Expected<S, std::string> Ex(Unexpected("long long long string"));
			ASSERT_THROW(Ex.Emplace(42), std::exception);
			ASSERT_EQ(Ex.Error(), "long long long string");
		}

		{
			Expected<S, std::string> Ex;
			ASSERT_THROW(Ex.Emplace(42), std::exception);
		}
	}
}