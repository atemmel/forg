#ifndef MATH_HPP
#define MATH_HPP

#include <map>
#include <vector>
#include <algorithm>
#include "array.hpp"

namespace cor {

	template<class T>
	constexpr const T& max_of(const T& a, const T& b) {
		return a < b ? b : a;
	}

	template<class T>
	const T& Max(const T& a, const T& b) {
		return a < b ? b : a;
	}

	template <typename T>
	constexpr T abs(T value) {
		return (value < 0) ? -value : value;
	}

	constexpr int sign(long long num);

	size_t pow(size_t base, size_t exponent);

	std::map<size_t, size_t> primefactorization(size_t n);

	size_t phiFunk(size_t n);

	template<class T, class ...Args>
	size_t lcm(const T num1, const T num2, Args... nums) {
		std::vector<std::map<size_t, size_t>>primeFactorVec = { primefactorization(num2), primefactorization(nums)... };
		auto newFactors = primefactorization(num1);
		size_t product = 1;
		for (auto &map : primeFactorVec) {

			for (auto &e : map) {
				if (newFactors.find(e.first) != newFactors.end())
				{
					if (e.second > newFactors.find(e.first)->second)
					{
						auto max = Max(e.second, newFactors.find(e.first)->second);
						newFactors[e.first] = max;
					}
				}
				else
				{
					newFactors.insert({ e.first,e.second });
				}
			}
		}
		for (auto &e : newFactors)
		{
			product *= cor::pow(e.first, e.second);
		}

		return product;
	}

	//size_t gcd(const int n1, const int n2) {
	//
	//	if (n2 == 0)
	//		return n1;
	//
	//	gcd(n2, n1 % n2);
	//}
	//
	//size_t egcd(const int n1, const int n2) {
	//
	//	if (n2 == 0)
	//		return n1;
	//}

	template<class>
	class Array;

	using Array2D = cor::Array<cor::Array<float>>;

	Array2D identityMatrix(size_t N);

	Array2D matrixAdd2D(Array2D& first, Array2D& second);

	Array2D matrixSub2D(Array2D& first, Array2D& second);

	Array2D matrixScalar2D(Array2D& first, int scalar);

	Array2D matrixMultip2D(Array2D& first, Array2D& second);

	Array2D matrixTransp2D(Array2D& first);

	Array2D matrixinvers2D(Array2D& first);

	int Det2x2(Array2D& first);
	int Det3x3(Array2D& first);

	//Array2D matrixDet2D(Array2D& first);

}// !cor::

#endif // !MATH_HPP
