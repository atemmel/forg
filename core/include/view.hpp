#pragma once

#include "safety.hpp"
#include "types.hpp"

namespace forg {

template <typename T>
class View {
public:
    constexpr View(const T* ptr, usize length) : ptr(ptr), length(length) {
    }

    constexpr View(const T* first, const T* last)
        : ptr(first), length(last - first) {
    }

    constexpr auto size() const -> usize {
        return length;
    }

    constexpr auto operator[](usize idx) const -> const T& {
        assert(idx < length);
        return ptr[idx];
    }

    constexpr auto begin() const -> T const* {
        return ptr;
    }

    constexpr auto end() const -> T const* {
        return ptr + size();
    }

    constexpr auto view(usize first, usize last) -> View<T> {
        assert(first <= size());
        assert(last <= size());
        assert(first <= last);
        return View(begin() + first, begin() + last);
    }

private:
    const T* ptr;
    usize length;
};

};  // namespace forg
