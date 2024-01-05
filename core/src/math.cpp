#include "math.hpp"

#include <cmath>

constexpr int cor::sign(long long num) {
    return num < 0 ? -1 : 1;
}

usize cor::pow(usize base, usize exponent) {
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

std::map<usize, usize> cor::primefactorization(usize n) {
    std::map<usize, usize> pFacMap;
    while (n % 2 == 0) {
        if (pFacMap.find(2) != pFacMap.end()) {
            pFacMap[2]++;
        } else {
            pFacMap.insert({2, 1});
        }
        n = n / 2;
    }

    for (usize i = 3; i <= sqrt(n); i++) {
        while (n % i == 0) {
            if (pFacMap.find(i) != pFacMap.end()) {
                pFacMap[i]++;
            } else {
                pFacMap.insert({i, 1});
            }
            n = n / i;
        }
    }

    if (n > 2) {
        if (pFacMap.find(n) != pFacMap.end()) {
            pFacMap[n]++;
        } else {
            pFacMap.insert({n, 1});
        }
    }
    return pFacMap;
}

usize cor::phiFunk(usize n) {
    auto factors  = primefactorization(n);
    usize product = 1;
    for (auto &e : factors) {
        product *= cor::pow(e.first, e.second - 1) * (e.first - 1);
    }

    return product;
}

cor::Array2D cor::identityMatrix(usize N) {
    cor::Array<float> col(N, 0);
    cor::Array2D newMatrix(N, col);

    for (usize row = 0; row < newMatrix.size(); row++) {
        for (usize col = 0; col < newMatrix.size(); col++) {
            if (row == col) {
                newMatrix[row][col] = 1;
            }
        }
    }
    return newMatrix;
}

cor::Array2D cor::matrixAdd2D(Array2D &first, Array2D &second) {
    cor::Array2D newMatrix(first.size());
    for (usize row = 0; row < first.size(); row++) {
        for (usize col = 0; col < first[row].size(); col++) {
            newMatrix[row].pushBack(first[row][col] + second[row][col]);
        }
    }
    return newMatrix;
}

cor::Array2D cor::matrixSub2D(Array2D &first, Array2D &second) {
    cor::Array2D newMatrix(first.size());
    for (usize row = 0; row < first.size(); row++) {
        for (usize col = 0; col < first[row].size(); col++) {
            newMatrix[row].pushBack(first[row][col] - second[row][col]);
        }
    }
    return newMatrix;
}

cor::Array2D cor::matrixScalar2D(Array2D &first, int scalar) {
    cor::Array2D newMatrix(first.size());
    for (usize row = 0; row < first.size(); row++) {
        for (usize col = 0; col < first[row].size(); col++) {
            newMatrix[row].pushBack(scalar * first[row][col]);
        }
    }
    return newMatrix;
}

cor::Array2D cor::matrixMultip2D(Array2D &first, Array2D &second) {
    cor::Array2D newMatrix(first.size());
    int sum = 0;
    for (usize row = 0; row < first.size(); row++) {
        for (usize col = 0; col < second[row].size(); col++) {
            float sum = 0;
            for (usize i = 0; i < first[row].size(); i++) {
                sum += first[row][i] * second[i][col];
            }
            newMatrix[row].pushBack(sum);
        }
    }
    return newMatrix;
}

cor::Array2D cor::matrixTransp2D(Array2D &first) {
    cor::Array2D newMatrix(first[0].size());
    for (usize row = 0; row < first.size(); row++) {
        for (usize col = 0; col < first[row].size(); col++) {
            newMatrix[col].pushBack(first[row][col]);
        }
    }
    return newMatrix;
}

cor::Array2D cor::matrixinvers2D(Array2D &matrix) {
    cor::Array2D newMatrix;
    // needs to be general for all matricies
    if (Det2x2(matrix) != 0) {
        newMatrix.resize(matrix.size());
        for (usize i = 0; i < matrix.size(); i++) {
            newMatrix[i] = matrix[i];
        }

        for (usize col = 0; col < newMatrix.size(); col++) {
            usize pivot_row = col;
            for (usize j = col + 1; j < newMatrix.size(); j++) {
                auto val1 = newMatrix[j][col], val2 = newMatrix[pivot_row][col];
                if (val1 > val2) {
                    pivot_row = j;
                }
            }

            if (pivot_row != col) {
                newMatrix[col].swap(newMatrix[pivot_row]);
            }
            auto pivot = newMatrix[col][col];
            for (usize i = col; i < newMatrix.size(); i++) {
                newMatrix[col][i] /= pivot;
            }

            for (usize row = col + 1; row < newMatrix.size(); row++) {
                auto ratio = newMatrix[row][col];
                for (usize k = col; k < newMatrix.size(); k++) {
                    newMatrix[row][k] -= newMatrix[col][k] * ratio;
                }
            }
        }
    }
    return newMatrix;
}

int cor::Det2x2(Array2D &mat) {
    return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

int cor::Det3x3(Array2D &mat) {
    return mat[0][0] * mat[1][1] * mat[2][2] +
           mat[0][1] * mat[1][2] * mat[2][0] +
           mat[0][2] * mat[1][0] * mat[2][1] -
           mat[0][2] * mat[1][1] * mat[2][0] -
           mat[0][1] * mat[1][0] * mat[2][2] -
           mat[0][0] * mat[1][2] * mat[2][1];
}
