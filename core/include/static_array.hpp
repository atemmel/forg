#pragma once

#include "slice.hpp"
#include "types.hpp"

namespace forg {

template <typename T, usize I>
class StaticArray {
public:
    constexpr StaticArray() {
    }

    constexpr auto size() const -> usize {
        return I;
    }

    constexpr auto operator[](usize i) const -> const T& {
        assert(i < size());
        return carray[i];
    }

    constexpr auto operator[](usize i) -> T& {
        assert(i < size());
        return carray[i];
    }

    constexpr auto begin() -> T* {
        return &carray;
    }

    constexpr auto end() -> T* {
        return &carray + size();
    }

    constexpr auto data() -> T* {
        return (T*)&carray;
    }

    constexpr auto begin() const -> T const* {
        return &carray;
    }

    constexpr auto end() const -> T const* {
        return &carray + size();
    }

    constexpr auto slice(usize first, usize last) -> Slice<T> {
        assert(first <= size());
        assert(last <= size());
        assert(first <= last);
        return Slice(data() + first, data() + last);
    }

    constexpr auto view(usize first, usize last) -> View<T> {
        assert(first <= size());
        assert(last <= size());
        assert(first <= last);
        return View(data() + first, data() + last);
    }

private:
    T carray[I];
};

}  // namespace forg
