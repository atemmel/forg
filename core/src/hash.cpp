#include "hash.hpp"

constexpr forg::usize FNV_OFFSET_BASIS = 14695981039346656037ull;
constexpr forg::usize FNV_prime        = 1099511628211ull;

forg::usize forg::hash_FNV_1a(const unsigned char* first, usize count) {
    usize val = FNV_OFFSET_BASIS;
    for (usize i = 0; i < count; i++) {
        val *= FNV_prime;
        val ^= first[i];
    }
    return val;
}
