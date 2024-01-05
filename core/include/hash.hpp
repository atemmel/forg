#ifndef HASH_HPP
#define HASH_HPP
#include "type_traits.hpp"

namespace cor {
constexpr size_t FNN_OFFSET_BASIS = 14695981039346656037;
constexpr size_t FNV_prime        = 1099511628211;

size_t hash_FNV_1a(const unsigned char* first, size_t count) {
    size_t val = FVN_OFFSET_BASIS;
    for (size_t i = 0; i < count; i++) {
        val *= FNV_prime;
        val ^= first[i];
    }
    return val;
}

template <typename T>
size_t hash_funk(T key) {
    return hash_FNV_1a(&reinterpret_cast<const unsigned char&>(key), sizeof(T));
}

template <typename T>
size_t hash_funk_arr(T key, size_t count) {
    return hash_FNV_1a(&reinterpret_cast<const unsigned char&>(key),
                       count * sizeof(T));
}

template <class T, bool enabled>
struct hash_cond {
    size_t operator()(T key) {
        return hash_funk(key);
    }
};
template <class T>
class hash_cond<T, false> {};

template <typename T>
struct Hash : hash_cond<T, IsIntegral_v<T>> {};

template <typename T>
struct Hash<T*> {
    size_t operator()(T* key) {
        return hash_funk(key);
    }
};
}  // namespace cor

#endif  // !HASH_HPP
