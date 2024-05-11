#include "algorithms.hpp"

usize forg::strlen(const char* str) {
    auto end = str;
    while (*end != '\0') {
        end++;
    }
    return static_cast<usize>(end - str);
}

int forg::strcmp(const char* lhs, const char* rhs) {
    if (lhs && rhs) {
        auto left = lhs, right = rhs;
        while (*left != '\0' && *right != '\0') {
            if (*left < *right) {
                return *left - *right;
            } else if (*left > *right) {
                return *left - *right;
            }
            left++;
            right++;
        }
        // equal 0
        if (*left == '\0' && *right == '\0') {
            return 0;
        }
        // right bigger -1
        else if (*left == '\0' && *right != '\0') {
            return -1;
        }
        // left bigger 1
        else if (*left != '\0' && *right == '\0') {
            return 1;
        }
    }
    return 1;
}
