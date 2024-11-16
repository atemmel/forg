#pragma once

#include "array.hpp"
#include "types.hpp"

namespace forg {

template <typename T>
constexpr T abs(T value) {
    return (value < 0) ? -value : value;
}

constexpr int sign(long long num);

usize pow(usize base, usize exponent);

template <class>
class Array;

using Array2D = forg::Array<forg::Array<float>>;

Array2D identityMatrix(usize N);

Array2D matrixAdd2D(Array2D& first, Array2D& second);

Array2D matrixSub2D(Array2D& first, Array2D& second);

Array2D matrixScalar2D(Array2D& first, int scalar);

Array2D matrixMultip2D(Array2D& first, Array2D& second);

Array2D matrixTransp2D(Array2D& first);

Array2D matrixinvers2D(Array2D& first);

int Det2x2(Array2D& first);
int Det3x3(Array2D& first);

// Array2D matrixDet2D(Array2D& first);

}  // namespace forg
