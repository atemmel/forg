#pragma once

#include "types.hpp"

namespace forg {

template <typename T>
constexpr T abs(T value) {
    return (value < 0) ? -value : value;
}

constexpr int sign(long long num);

usize pow(usize base, usize exponent);

}  // namespace forg
