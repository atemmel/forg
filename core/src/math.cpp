#include "math.hpp"

#include <cmath>

namespace forg {

constexpr int sign(long long num) {
    return num < 0 ? -1 : 1;
}

usize pow(usize base, usize exponent) {
    usize product = 1;
    if (exponent == 2) {
        product = base * base;
    } else {
        for (usize i = 1; i <= exponent; i++) {
            product *= base;
        }
    }
    return product;
}

}  // namespace forg
