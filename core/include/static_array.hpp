#pragma once

#include "safety.hpp"
#include "slice.hpp"
#include "types.hpp"

template <typename T, usize I>
class StaticArray {
   public:
    constexpr StaticArray() {
    }

    constexpr auto size() const -> usize {
        return I;
    }

    constexpr auto begin() -> T* {
        return &carray;
    }

    constexpr auto end() -> T* {
        return &carray + size();
    }

    constexpr auto begin() const -> T const* {
        return &carray;
    }

    constexpr auto end() const -> T const* {
        return &carray + size();
    }

    constexpr auto operator[](usize i) const -> const T& {
        assert(i < size());
        return carray[i];
    }

    constexpr auto operator[](usize i) -> T& {
        assert(i < size());
        return carray[i];
    }

    constexpr auto slice(usize first, usize last) -> Slice<T> {
        assert(first <= size());
        assert(last <= size());
        assert(first <= last);
        return Slice(begin() + first, begin() + last);
    }

   private:
    T carray[I];
};
