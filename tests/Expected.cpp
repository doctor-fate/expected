#include <memory>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include <Expected/Expected.hpp>

#include "Utility.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace stdx::tests
{
    TEST(Expected, Constructor_Default)
    {
        {
            Expected<void, double> Ex;
            ASSERT_TRUE(Ex.HasValue());
            ASSERT_TRUE(bool(Ex));
        }

        {
            Expected<int, double> Ex;
            ASSERT_TRUE(Ex.HasValue());
            ASSERT_TRUE(bool(Ex));
            ASSERT_EQ(*Ex, 0);
        }

        {
            Expected<std::vector<int>, std::string> Ex;
            ASSERT_TRUE(Ex.HasValue());
            ASSERT_TRUE(bool(Ex));
            ASSERT_TRUE(empty(*Ex));
        }

        {
            static_assert(!std::is_default_constructible_v<Expected<NonDefaultConstructible, double>>);
        }
    }

    TEST(Expected, Constructor_FromU)
    {
        {
            Expected<int, double> Ex1 = 42;
            ASSERT_EQ(*Ex1, 42);
        }

        {
            Expected<std::vector<int>, std::string> Ex(3);
            ASSERT_EQ(size(*Ex), 3);
            using T = Expected<std::vector<int>, std::string>;
            static_assert(!IsImplicitlyConstructible<T, int>());
        }
    }

    TEST(Expected, Constructor_CopyFromExpected)
    {
        {
            Expected<int, double> Ex1 = 42, Ex2 = Unexpected(42.0);
            Expected<long, float> Ex3 = Ex1, Ex4 = Ex2;
            ASSERT_EQ(*Ex3, 42);
            ASSERT_EQ(Ex4.Error(), 42.0f);
        }

        {
            Expected<int, std::string> Ex1 = 42, Ex2 = Unexpected("hello");
            Expected<std::vector<int>, std::string_view> Ex3(Ex1), Ex4(Ex2);
            ASSERT_EQ(size(*Ex3), 42);
            ASSERT_EQ(Ex4.Error(), "hello");
            using T1 = Expected<std::vector<int>, std::string_view>;
            using T2 = Expected<int, std::string>;
            static_assert(!IsImplicitlyConstructible<T1, const T2&>());
        }
    }

    TEST(Expected, Constructor_MoveFromExpected)
    {
        {
            Expected<int, double> Ex1 = 42, Ex2 = Unexpected(42.0);
            Expected<long, float> Ex3 = std::move(Ex1), Ex4 = std::move(Ex2);
            ASSERT_EQ(*Ex3, 42);
            ASSERT_EQ(Ex4.Error(), 42.0f);
        }

        {
            Expected<int*, std::string> Ex1 = new int(42), Ex2 = Unexpected("hello");
            Expected<std::unique_ptr<int>, std::string_view> Ex3(std::move(Ex1)), Ex4(std::move(Ex2));
            ASSERT_EQ(**Ex3, 42);
            ASSERT_EQ(Ex4.Error(), "hello");
            using T1 = Expected<std::unique_ptr<int>, std::string_view>;
            using T2 = Expected<int, std::string>;
            static_assert(!IsImplicitlyConstructible<T1, const T2&>());
            static_assert(!IsExplicitlyConstructible<T1, const T2&>());
            static_assert(!IsImplicitlyConstructible<T1, T2&&>());
        }
    }

    TEST(Expected, Constructor_CopyFromUnexpected)
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
            using T = Expected<int, std::vector<int>>;
            static_assert(!IsImplicitlyConstructible<T, const Unexpected<int>>());
        }
    }

    TEST(Expected, Constructor_MoveFromUnexpected)
    {
        {
            Expected<int, std::string> Ex = Unexpected("unexpected");
            ASSERT_EQ(Ex.Error(), "unexpected");
        }

        {
            Expected<int, std::unique_ptr<int>> Ex(Unexpected(new int(42)));
            ASSERT_EQ(*Ex.Error(), 42);
            using T = Expected<int, std::unique_ptr<int>>;
            static_assert(!IsImplicitlyConstructible<T, Unexpected<int*>&&>());
        }
    }

    TEST(Expected, Constructor_InPlace)
    {
        {
            Expected<std::pair<int, std::string>, std::string> Ex(
                std::in_place, std::piecewise_construct, std::forward_as_tuple(42), std::forward_as_tuple(4, 'a'));
            ASSERT_EQ(Ex->first, 42);
            ASSERT_EQ(Ex->second, "aaaa");
        }

        {
            Expected<std::vector<int>, std::string> Ex(std::in_place, {1, 2, 3, 4}, std::allocator<int>{});
            ASSERT_EQ(*Ex, (std::vector<int>{1, 2, 3, 4}));
        }
    }

    TEST(Expected, Constructor_Unexpect)
    {
        {
            Expected<std::string, std::pair<int, std::string>> Ex(
                unexpect, std::piecewise_construct, std::forward_as_tuple(42), std::forward_as_tuple(4, 'a'));
            ASSERT_EQ(Ex.Error().first, 42);
            ASSERT_EQ(Ex.Error().second, "aaaa");
        }

        {
            Expected<std::vector<int>, std::string> Ex(unexpect, {'a', 'b', 'c', 'd'}, std::allocator<char>{});
            ASSERT_EQ(Ex.Error(), "abcd");
        }
    }

    TEST(Expected, Constructor_Copy)
    {
        {
            Expected<std::vector<int>, std::string> Ex1(42);
            Expected<std::vector<int>, std::string> Ex2 = Ex1;
            ASSERT_EQ(Ex1, Ex2);
        }

        {
            Expected<std::vector<int>, std::string> Ex1 = Unexpected("hello");
            Expected<std::vector<int>, std::string> Ex2 = Ex1;
            ASSERT_EQ(Ex1, Ex2);
        }

        {
            Expected<void, std::string> Ex1;
            Expected<void, std::string> Ex2 = Ex1;
            ASSERT_TRUE(Ex2.HasValue());
        }

        {
            Expected<void, std::string> Ex1 = Unexpected("hello");
            Expected<void, std::string> Ex2 = Ex1;
            ASSERT_FALSE(Ex2.HasValue());
        }
    }

    TEST(Expected, Constructor_Move)
    {
        {
            Expected<void, std::unique_ptr<int>> Ex1(Unexpected(new int(42)));
            Expected<void, std::unique_ptr<int>> Ex2 = std::move(Ex1);
            ASSERT_EQ(*Ex2.Error(), 42);
        }

        {
            Expected<std::vector<int>, std::string> Ex1(42);
            Expected<std::vector<int>, std::string> Ex2 = std::move(Ex1);
            ASSERT_EQ(size(*Ex2), 42);
        }

        {
            Expected<std::unique_ptr<int>, std::string> Ex1 = std::make_unique<int>(42);
            Expected<std::unique_ptr<int>, std::string> Ex2 = std::move(Ex1);
            ASSERT_EQ(**Ex2, 42);
        }
    }

    TEST(Expected, Assign_CopyUnexpected)
    {
        {
            Unexpected Unex("hello");
            Expected<std::vector<int>, std::string_view> Ex(42);
            Ex = Unex;
            ASSERT_EQ(Ex.Error(), "hello");
        }

        {
            Unexpected Unex("world");
            Expected<void, std::string_view> Ex(Unexpected("hello"));
            Ex = Unex;
            ASSERT_EQ(Ex.Error(), "world");
        }

        {
            Unexpected Unex("hello");
            Expected<void, std::string> Ex;
            Ex = Unex;
            ASSERT_EQ(Ex.Error(), "hello");
        }
    }

    TEST(Expected, Assign_MoveUnexpected)
    {
        {
            Expected<std::vector<int>, std::string> Ex(42);
            Ex = Unexpected<std::string>("hello");
            ASSERT_EQ(Ex.Error(), "hello");
        }

        {
            Expected<void, std::string> Ex(Unexpected("hello"));
            Ex = Unexpected<std::string>("world");
            ASSERT_EQ(Ex.Error(), "world");
        }

        {
            Unexpected Unex("hello"s);
            Expected<void, std::string> Ex;
            Ex = std::move(Unex);
            ASSERT_EQ(Ex.Error(), "hello");
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
            Expected<std::vector<int>, std::string> Ex(Unexpected("hello"));
            Ex = std::vector<int>{1, 2, 3, 4};
            ASSERT_EQ(*Ex, (std::vector<int>{1, 2, 3, 4}));
        }

        {
            Expected<ThrowableMoveConstructible, std::string> Ex(Unexpected("hello"));
            Ex = ThrowableMoveConstructible("world", false);
            ASSERT_EQ(Ex->Value, "world");
        }

        {
            Expected<ThrowableMoveConstructible, std::string> Ex(Unexpected("long long long string"));
            ASSERT_THROW(Ex = ThrowableMoveConstructible("world", true), std::logic_error);
            ASSERT_EQ(Ex.Error(), "long long long string");
        }
    }

    TEST(Expected, Assign_Copy)
    {
        {
            std::vector<int> Vector{1, 2, 3, 4};
            Expected<std::vector<int>, std::string> Ex1(Vector);
            Expected<std::vector<int>, std::string> Ex2;
            Ex2 = Ex1;
            ASSERT_EQ(Ex2, Ex1);
        }

        {
            Expected<void, std::string> Ex1;
            Expected<void, std::string> Ex2 = Unexpected("hello");
            Ex2 = Ex1;
            ASSERT_TRUE(Ex2.HasValue());
        }

        {
            Expected<int, std::string> Ex1 = 42;
            Expected<int, std::string> Ex2 = Unexpected("hello");
            Ex2 = Ex1;
            ASSERT_EQ(Ex2, Ex1);
        }

        {
            Expected<std::vector<int>, std::string> Ex1(std::in_place, {1, 2, 3, 4});
            Expected<std::vector<int>, std::string> Ex2 = Unexpected("hello");
            Ex2 = Ex1;
            ASSERT_EQ(Ex2, Ex1);
        }

        {
            Expected<ThrowableCopyConstructible, std::string> Ex1(std::in_place, "world", false);
            Expected<ThrowableCopyConstructible, std::string> Ex2 = Unexpected("hello");
            Ex2 = Ex1;
            ASSERT_EQ(Ex2, Ex1);
        }

        {
            Expected<ThrowableCopyConstructible, std::string> Ex1(std::in_place, "world", true);
            Expected<ThrowableCopyConstructible, std::string> Ex2 = Unexpected("long long long string");
            ASSERT_THROW(Ex2 = Ex1, std::logic_error);
            ASSERT_EQ(Ex2.Error(), "long long long string");
        }

        {
            Expected<std::vector<int>, std::string> Ex1 = Unexpected("hello");
            Expected<std::vector<int>, std::string> Ex2 = Unexpected("world");
            Ex2 = Ex1;
            ASSERT_EQ(Ex2, Ex1);
        }

        {
            Expected<void, std::string> Ex1 = Unexpected("hello");
            Expected<void, std::string> Ex2;
            Ex2 = Ex1;
            ASSERT_EQ(Ex2.Error(), "hello");
        }

        {
            Expected<std::string, int> Ex1 = Unexpected(42);
            Expected<std::string, int> Ex2 = "long long long string";
            Ex2 = Ex1;
            ASSERT_EQ(Ex2, Ex1);
        }

        {
            Expected<int, std::string> Ex1 = Unexpected("hello");
            Expected<int, std::string> Ex2 = 42;
            Ex2 = Ex1;
            ASSERT_EQ(Ex2, Ex1);
        }

        {
            Expected<std::string, ThrowableCopyConstructible> Ex1(unexpect, "world", false);
            Expected<std::string, ThrowableCopyConstructible> Ex2 = "long long long string";
            Ex2 = Ex1;
            ASSERT_EQ(Ex2, Ex1);
        }

        {
            Expected<std::string, ThrowableCopyConstructible> Ex1(unexpect, "world", true);
            Expected<std::string, ThrowableCopyConstructible> Ex2 = "long long long string";
            ASSERT_THROW(Ex2 = Ex1, std::logic_error);
            ASSERT_EQ(*Ex2, "long long long string");
        }
    }

    TEST(Expected, Assign_Move)
    {
        {
            std::vector<int> Vector{1, 2, 3, 4};
            Expected<std::vector<int>, std::string> Ex1(Vector);
            Expected<std::vector<int>, std::string> Ex2;
            Ex2 = std::move(Ex1);
            ASSERT_EQ(*Ex2, Vector);
        }

        {
            Expected<void, std::string> Ex1;
            Expected<void, std::string> Ex2 = Unexpected("hello");
            Ex2 = std::move(Ex1);
            ASSERT_TRUE(Ex2.HasValue());
        }

        {
            std::vector<int> Vector{1, 2, 3, 4};
            Expected<std::vector<int>, std::string> Ex1 = Vector;
            Expected<std::vector<int>, std::string> Ex2 = Unexpected("hello");
            Ex2 = std::move(Ex1);
            ASSERT_EQ(*Ex2, Vector);
        }

        {
            Expected<ThrowableMoveConstructible, std::string> Ex1(std::in_place, "world", false);
            Expected<ThrowableMoveConstructible, std::string> Ex2 = Unexpected("hello");
            Ex2 = std::move(Ex1);
            ASSERT_EQ(Ex2->Value, "world");
        }

        {
            Expected<ThrowableMoveConstructible, std::string> Ex1(std::in_place, "world", true);
            Expected<ThrowableMoveConstructible, std::string> Ex2 = Unexpected("long long long string");
            ASSERT_THROW(Ex2 = std::move(Ex1), std::logic_error);
            ASSERT_EQ(Ex2.Error(), "long long long string");
        }

        {
            Expected<std::vector<int>, std::string> Ex1 = Unexpected("hello");
            Expected<std::vector<int>, std::string> Ex2 = Unexpected("world");
            Ex2 = std::move(Ex1);
            ASSERT_EQ(Ex2.Error(), "hello");
        }

        {
            Expected<void, std::string> Ex1 = Unexpected("hello");
            Expected<void, std::string> Ex2;
            Ex2 = std::move(Ex1);
            ASSERT_EQ(Ex2.Error(), "hello");
        }

        {
            Expected<int, std::string> Ex1 = Unexpected("hello");
            Expected<int, std::string> Ex2 = 42;
            Ex2 = std::move(Ex1);
            ASSERT_EQ(Ex2.Error(), "hello");
        }

        {
            Expected<std::string, ThrowableMoveConstructible> Ex1(unexpect, "world", false);
            Expected<std::string, ThrowableMoveConstructible> Ex2 = "long long long string";
            Ex2 = std::move(Ex1);
            ASSERT_EQ(Ex2.Error().Value, "world");
        }

        {
            Expected<std::string, ThrowableMoveConstructible> Ex1(unexpect, "world", true);
            Expected<std::string, ThrowableMoveConstructible> Ex2 = "long long long string";
            ASSERT_THROW(Ex2 = std::move(Ex1), std::logic_error);
            ASSERT_EQ(*Ex2, "long long long string");
        }
    }

    TEST(Expected, Swap)
    {
        using std::swap;

        {
            std::vector<int> Vector1{1, 2, 3, 4}, Vector2{5, 6, 7, 8};
            Expected<std::vector<int>, std::string> Ex1(Vector1);
            Expected<std::vector<int>, std::string> Ex2(Vector2);
            swap(Ex1, Ex2);
            ASSERT_EQ(Ex1.Value(), Vector2);
            ASSERT_EQ(Ex2.Value(), Vector1);
        }

        {
            Expected<std::vector<int>, std::string> Ex1 = Unexpected("hello");
            Expected<std::vector<int>, std::string> Ex2 = Unexpected("world");
            swap(Ex1, Ex2);
            ASSERT_FALSE(Ex1.HasValue());
            ASSERT_FALSE(Ex2.HasValue());
            ASSERT_EQ(Ex1.Error(), "world");
            ASSERT_EQ(Ex2.Error(), "hello");
        }

        {
            std::vector<int> Vector1{1, 2, 3, 4};
            Expected<std::vector<int>, std::string> Ex1(Vector1);
            Expected<std::vector<int>, std::string> Ex2(Unexpected("hello"));

            swap(Ex1, Ex2);
            ASSERT_FALSE(Ex1.HasValue());
            ASSERT_EQ(Ex1.Error(), "hello");
            ASSERT_EQ(Ex2.Value(), Vector1);

            swap(Ex1, Ex2);
            ASSERT_EQ(Ex1.Value(), Vector1);
            ASSERT_FALSE(Ex2.HasValue());
            ASSERT_EQ(Ex2.Error(), "hello");
        }

        {
            Expected<void, std::string> Ex1;
            Expected<void, std::string> Ex2 = Unexpected("world");
            swap(Ex1, Ex2);
            ASSERT_FALSE(Ex1.HasValue());
            ASSERT_EQ(Ex1.Error(), "world");
            ASSERT_TRUE(Ex2.HasValue());
        }

        {
            Expected<int, std::string> Ex1 = 42;
            Expected<int, std::string> Ex2 = Unexpected("world");
            swap(Ex1, Ex2);
            ASSERT_FALSE(Ex1.HasValue());
            ASSERT_EQ(Ex1.Error(), "world");
            ASSERT_EQ(Ex2.Value(), 42);
        }

        {
            Expected<ThrowableMoveConstructible, std::string> Ex1(std::in_place, "hello", false);
            Expected<ThrowableMoveConstructible, std::string> Ex2 = Unexpected("world");
            swap(Ex1, Ex2);
            ASSERT_FALSE(Ex1.HasValue());
            ASSERT_EQ(Ex1.Error(), "world");
            ASSERT_EQ(Ex2.Value().Value, "hello");
        }

        {
            Expected<ThrowableMoveConstructible, std::string> Ex1(std::in_place, "long long long string", true);
            Expected<ThrowableMoveConstructible, std::string> Ex2 = Unexpected("hello beautiful world");
            ASSERT_THROW(swap(Ex1, Ex2), std::logic_error);
            ASSERT_EQ(Ex1.Value().Value, "long long long string");
            ASSERT_FALSE(Ex2.HasValue());
            ASSERT_EQ(Ex2.Error(), "hello beautiful world");
        }

        {
            Expected<std::string, ThrowableMoveConstructible> Ex1 = "hello beautiful world";
            Expected<std::string, ThrowableMoveConstructible> Ex2(unexpect, "long long long string", false);
            swap(Ex1, Ex2);
            ASSERT_FALSE(Ex1.HasValue());
            ASSERT_EQ(Ex1.Error().Value, "long long long string");
            ASSERT_EQ(Ex2.Value(), "hello beautiful world");
        }

        {
            Expected<std::string, ThrowableMoveConstructible> Ex1 = "hello beautiful world";
            Expected<std::string, ThrowableMoveConstructible> Ex2(unexpect, "long long long string", true);
            ASSERT_THROW(swap(Ex1, Ex2), std::logic_error);
            ASSERT_EQ(Ex1.Value(), "hello beautiful world");
            ASSERT_FALSE(Ex2.HasValue());
            ASSERT_EQ(Ex2.Error().Value, "long long long string");
        }
    }

    TEST(Expected, Emplace)
    {
        {
            Expected<int, std::string> Ex;
            Ex.Emplace(42);
            ASSERT_EQ(*Ex, 42);
        }

        {
            Expected<std::vector<int>, std::string> Ex;
            Ex.Emplace({1, 2, 3, 4}, std::allocator<int>{});
            ASSERT_EQ(*Ex, (std::vector<int>{1, 2, 3, 4}));
        }

        {
            Expected<int, std::string> Ex = Unexpected("hello");
            Ex.Emplace(42);
            ASSERT_EQ(*Ex, 42);
        }

        {
            Expected<std::vector<int>, std::string> Ex = Unexpected("hello");
            Ex.Emplace({1, 2, 3, 4}, std::allocator<int>{});
            ASSERT_EQ(*Ex, (std::vector<int>{1, 2, 3, 4}));
        }

        {
            Expected<ThrowableEmplaceConstructible, std::string> Ex = Unexpected("hello");
            Ex.Emplace(90, 78.0, "hello");
            ASSERT_TRUE(Ex.HasValue());
        }

        {
            Expected<ThrowableEmplaceConstructible, std::string> Ex = Unexpected("hello");
            ASSERT_THROW(Ex.Emplace(42, 78.0, "hello"), std::logic_error);
            ASSERT_EQ(Ex.Error(), "hello");
        }

        {
            Expected<void, std::string> Ex;
            Ex.Emplace();
            ASSERT_TRUE(Ex.HasValue());
        }

        {
            Expected<void, std::string> Ex = Unexpected("long long long string");
            Ex.Emplace();
            ASSERT_TRUE(Ex.HasValue());
        }
    }

    TEST(Expected, EqCompare)
    {
        stdx::Expected<int, std::string> Ex1(42), Ex2(56), Ex3(unexpect, "hello"), Ex4(unexpect, "world");

        ASSERT_TRUE(Ex1 == Ex1);
        ASSERT_TRUE(Ex3 == Ex3);
        ASSERT_FALSE(Ex1 == Ex2);
        ASSERT_FALSE(Ex1 == Ex3);
        ASSERT_FALSE(Ex3 == Ex4);
        ASSERT_FALSE(Ex3 == Ex1);

        ASSERT_FALSE(Ex1 != Ex1);
        ASSERT_FALSE(Ex3 != Ex3);
        ASSERT_TRUE(Ex1 != Ex2);
        ASSERT_TRUE(Ex1 != Ex3);
        ASSERT_TRUE(Ex3 != Ex4);
        ASSERT_TRUE(Ex3 != Ex1);

        ASSERT_TRUE(Ex1 == 42);
        ASSERT_TRUE(42 == Ex1);
        ASSERT_FALSE(Ex1 == 56);
        ASSERT_FALSE(Ex3 == 42);

        ASSERT_FALSE(Ex1 != 42);
        ASSERT_FALSE(42 != Ex1);
        ASSERT_TRUE(Ex1 != 56);
        ASSERT_TRUE(Ex3 != 42);

        ASSERT_TRUE(Ex3 == Unexpected("hello"));
        ASSERT_TRUE(Unexpected("hello") == Ex3);
        ASSERT_FALSE(Ex3 == Unexpected("world"));
        ASSERT_FALSE(Ex1 == Unexpected("hello"));

        ASSERT_FALSE(Ex3 != Unexpected("hello"));
        ASSERT_FALSE(Unexpected("hello") != Ex3);
        ASSERT_TRUE(Ex3 != Unexpected("world"));
        ASSERT_TRUE(Ex1 != Unexpected("hello"));
    }

    TEST(Expected, Accessors)
    {
        {
            Expected<void, std::string> Ex;

            ASSERT_TRUE(Ex.HasValue());
            ASSERT_TRUE(bool(Ex));

            Ex = Unexpected("hello");

            ASSERT_FALSE(Ex.HasValue());
            ASSERT_FALSE(bool(Ex));
        }

        {
            Expected<std::string, int> Ex = "hello";

            ASSERT_TRUE(Ex.HasValue());
            ASSERT_TRUE(bool(Ex));

            Ex = Unexpected(42);

            ASSERT_FALSE(Ex.HasValue());
            ASSERT_FALSE(bool(Ex));
        }

        {
            Expected<void, std::string> Ex1 = Unexpected("hello");
            ASSERT_EQ(Ex1.Error(), "hello");
            const Expected<void, std::string> Ex2 = Unexpected(std::move(Ex1).Error());
            ASSERT_EQ(Ex2.Error(), "hello");
            ASSERT_EQ(std::move(Ex2).Error(), "hello");
        }

        {
            Expected<std::string, int> Ex1 = Unexpected(42);
            ASSERT_THROW(Ex1.Value(), BadExpectedAccess<int>);
            ASSERT_THROW(std::move(Ex1).Value(), BadExpectedAccess<int>);
            const Expected<std::string, int> Ex2 = Unexpected(42);
            ASSERT_THROW(Ex2.Value(), BadExpectedAccess<int>);
            ASSERT_THROW(std::move(Ex2).Value(), BadExpectedAccess<int>);
        }

        {
            Expected<void, int> Ex1;
            ASSERT_NO_THROW(Ex1.Value());
            ASSERT_NO_THROW(std::move(Ex1).Value());
            const Expected<void, int> Ex2;
            ASSERT_NO_THROW(Ex2.Value());
            ASSERT_NO_THROW(std::move(Ex2).Value());
        }

        {
            Expected<void, int> Ex1 = Unexpected(42);
            ASSERT_THROW(Ex1.Value(), BadExpectedAccess<int>);
            ASSERT_THROW(std::move(Ex1).Value(), BadExpectedAccess<int>);
            const Expected<void, int> Ex2 = Unexpected(42);
            ;
            ASSERT_THROW(Ex2.Value(), BadExpectedAccess<int>);
            ASSERT_THROW(std::move(Ex2).Value(), BadExpectedAccess<int>);
        }

        {
            Expected<std::string, int> Ex1 = "hello";
            ASSERT_EQ(Ex1.Value(), "hello");
            ASSERT_EQ(std::move(Ex1).Value(), "hello");
            const Expected<std::string, int> Ex2 = "hello";
            ASSERT_EQ(Ex2.Value(), "hello");
            ASSERT_EQ(std::move(Ex2).Value(), "hello");
        }

        {
            Expected<std::string, int> Ex1 = "hello";
            ASSERT_EQ(*Ex1, "hello");
            ASSERT_EQ(*std::move(Ex1), "hello");
            const Expected<std::string, int> Ex2 = "hello";
            ASSERT_EQ(*Ex2, "hello");
            ASSERT_EQ(*std::move(Ex2), "hello");
        }

        {
            Expected<std::string, int> Ex1 = "hello";
            Ex1->append(", world");
            ASSERT_EQ(*Ex1, "hello, world");
            const Expected<std::string, int> Ex2 = "hello";
            ASSERT_EQ(Ex2->size(), 5);
        }

        {
            Expected<std::string, int> Ex1 = "hello";
            ASSERT_EQ(Ex1.ValueOr("world"), "hello");
            Ex1 = Unexpected(42);
            ASSERT_EQ(Ex1.ValueOr("world"), "world");

            Expected<std::string, int> Ex2 = "hello";
            ASSERT_EQ(std::move(Ex2).ValueOr("world"), "hello");
            Ex2 = Unexpected(42);
            ASSERT_EQ(std::move(Ex2).ValueOr("world"), "world");
        }
    }
}