#include "ExpectedStorage.hpp"

namespace stdx::details {
    template <typename T, typename E, bool Trivial = VoidOrTriviallyDestructible<T>() && TriviallyDestructible<E>()>
    struct BaseDestructor : ExpectedStorage<T, E> {
        using Super = ExpectedStorage<T, E>;
        using Super::Super;

        ~BaseDestructor() {
            if (Super::HasValue()) {
                if constexpr (!VoidOrTriviallyDestructible<T>()) {
                    Super::DestroyValue();
                }
            } else {
                if constexpr (!TriviallyDestructible<E>()) {
                    Super::DestroyUnexpected();
                }
            }
        }
    };

    template <typename T, typename E>
    struct BaseDestructor<T, E, true> : ExpectedStorage<T, E> {
        using Super = ExpectedStorage<T, E>;
        using Super::Super;
    };
}