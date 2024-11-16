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

Array2D identityMatrix(usize N) {
    Array<float> col(N, 0);
    Array2D newMatrix(N, col);

    for (usize row = 0; row < newMatrix.size(); row++) {
        for (usize col = 0; col < newMatrix.size(); col++) {
            if (row == col) {
                newMatrix[row][col] = 1;
            }
        }
    }
    return newMatrix;
}

Array2D matrixAdd2D(Array2D &first, Array2D &second) {
    Array2D newMatrix(first.size());
    for (usize row = 0; row < first.size(); row++) {
        for (usize col = 0; col < first[row].size(); col++) {
            newMatrix[row].pushBack(first[row][col] + second[row][col]);
        }
    }
    return newMatrix;
}

Array2D matrixSub2D(Array2D &first, Array2D &second) {
    Array2D newMatrix(first.size());
    for (usize row = 0; row < first.size(); row++) {
        for (usize col = 0; col < first[row].size(); col++) {
            newMatrix[row].pushBack(first[row][col] - second[row][col]);
        }
    }
    return newMatrix;
}

Array2D matrixScalar2D(Array2D &first, int scalar) {
    Array2D newMatrix(first.size());
    for (usize row = 0; row < first.size(); row++) {
        for (usize col = 0; col < first[row].size(); col++) {
            newMatrix[row].pushBack(scalar * first[row][col]);
        }
    }
    return newMatrix;
}

Array2D matrixMultip2D(Array2D &first, Array2D &second) {
    Array2D newMatrix(first.size());
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

Array2D matrixTransp2D(Array2D &first) {
    Array2D newMatrix(first[0].size());
    for (usize row = 0; row < first.size(); row++) {
        for (usize col = 0; col < first[row].size(); col++) {
            newMatrix[col].pushBack(first[row][col]);
        }
    }
    return newMatrix;
}

Array2D matrixinvers2D(Array2D &matrix) {
    Array2D newMatrix;
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

int Det2x2(Array2D &mat) {
    return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

int Det3x3(Array2D &mat) {
    return mat[0][0] * mat[1][1] * mat[2][2] +
           mat[0][1] * mat[1][2] * mat[2][0] +
           mat[0][2] * mat[1][0] * mat[2][1] -
           mat[0][2] * mat[1][1] * mat[2][0] -
           mat[0][1] * mat[1][0] * mat[2][2] -
           mat[0][0] * mat[1][2] * mat[2][1];
}

}  // namespace forg
