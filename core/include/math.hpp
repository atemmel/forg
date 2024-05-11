#ifndef MATH_HPP
#define MATH_HPP

#include <algorithm>
#include <map>
#include <vector>

#include "array.hpp"
#include "types.hpp"

namespace forg {

template <typename T>
constexpr T abs(T value) {
    return (value < 0) ? -value : value;
}

constexpr int sign(long long num);

usize pow(usize base, usize exponent);

std::map<usize, usize> primefactorization(usize n);

usize phiFunk(usize n);

template <class T, class... Args>
usize lcm(const T num1, const T num2, Args... nums) {
    std::vector<std::map<usize, usize>> primeFactorVec = {
        primefactorization(num2), primefactorization(nums)...};
    auto newFactors = primefactorization(num1);
    usize product   = 1;
    for (auto& map : primeFactorVec) {
        for (auto& e : map) {
            if (newFactors.find(e.first) != newFactors.end()) {
                if (e.second > newFactors.find(e.first)->second) {
                    auto max = Max(e.second, newFactors.find(e.first)->second);
                    newFactors[e.first] = max;
                }
            } else {
                newFactors.insert({e.first, e.second});
            }
        }
    }
    for (auto& e : newFactors) {
        product *= forg::pow(e.first, e.second);
    }

    return product;
}

// usize gcd(const int n1, const int n2) {
//
//	if (n2 == 0)
//		return n1;
//
//	gcd(n2, n1 % n2);
// }
//
// usize egcd(const int n1, const int n2) {
//
//	if (n2 == 0)
//		return n1;
// }

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

#endif  // !MATH_HPP
