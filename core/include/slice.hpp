#pragma once

#include "types.hpp"

template <typename T>
class Slice {
   public:
    constexpr Slice(T* ptr, usize length) : ptr(ptr), length(length) {
    }

    constexpr Slice(T* first, T* last) : ptr(first), length(last - first) {
    }

    constexpr auto size() const -> usize {
        return length;
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

   private:
    T* ptr;
    usize length;
};
