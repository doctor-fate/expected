#include <algorithm>
#include <memory>
#include <string>

#include <Expected/Expected.hpp>

int main() {
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
        struct NonSuch {
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