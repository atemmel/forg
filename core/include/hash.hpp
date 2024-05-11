#ifndef HASH_HPP
#define HASH_HPP
#include "type_traits.hpp"

namespace forg {
constexpr usize FNV_OFFSET_BASIS = 14695981039346656037ull;
constexpr usize FNV_prime        = 1099511628211ull;

usize hash_FNV_1a(const unsigned char* first, usize count) {
    usize val = FNV_OFFSET_BASIS;
    for (usize i = 0; i < count; i++) {
        val *= FNV_prime;
        val ^= first[i];
    }
    return val;
}

template <typename T>
usize hash_funk(T key) {
    return hash_FNV_1a(&reinterpret_cast<const unsigned char&>(key), sizeof(T));
}

template <typename T>
usize hash_funk_arr(T key, usize count) {
    return hash_FNV_1a(&reinterpret_cast<const unsigned char&>(key),
                       count * sizeof(T));
}

template <class T, bool enabled>
struct hash_cond {
    usize operator()(T key) {
        return hash_funk(key);
    }
};
template <class T>
class hash_cond<T, false> {};

template <typename T>
struct Hash : hash_cond<T, IsIntegral_v<T>> {};

template <typename T>
struct Hash<T*> {
    usize operator()(T* key) {
        return hash_funk(key);
    }
};
}  // namespace forg

#endif  // !HASH_HPP
