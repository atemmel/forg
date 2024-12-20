#pragma once

#include "safety.hpp"
#include "types.hpp"
#include "view.hpp"

namespace forg {

template <typename T>
class Slice {
public:
    constexpr Slice(const T* ptr, usize length) : ptr(ptr), length(length) {
    }

    constexpr Slice(T* ptr, usize length) : ptr(ptr), length(length) {
    }

    constexpr Slice(const T* first, const T* last)
        : ptr(first), length(last - first) {
    }

    constexpr Slice(T* first, T* last) : ptr(first), length(last - first) {
    }

    constexpr auto size() const -> usize {
        return length;
    }

    constexpr auto operator[](usize idx) const -> const T& {
        assert(idx < length);
        return ptr[idx];
    }

    constexpr auto operator[](usize idx) -> T& {
        assert(idx < length);
        return ptr[idx];
    }

    constexpr auto begin() -> T* {
        return ptr;
    }

    constexpr auto end() -> T* {
        return ptr + size();
    }

    constexpr auto begin() const -> T const* {
        return ptr;
    }

    constexpr auto end() const -> T const* {
        return ptr + size();
    }

    constexpr auto slice(usize first, usize last) -> Slice<T> {
        assert(first <= size());
        assert(last <= size());
        assert(first <= last);
        return Slice(begin() + first, begin() + last);
    }

    constexpr auto view(usize first, usize last) -> View<T> {
        return View(ptr, length);
    }

private:
    T* ptr;
    usize length;
};

};  // namespace forg
