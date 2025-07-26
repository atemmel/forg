#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "type_traits.hpp"

namespace forg {

template <class T>
constexpr const T& max(const T& a, const T& b) {
    return a > b ? a : b;
}

template <class T>
constexpr const T& min(const T& a, const T& b) {
    return a < b ? a : b;
}

template <typename T>
constexpr forg::RemoveReference_T<T>&& isMovable(T&& t) noexcept {
    return static_cast<forg::RemoveReference_T<T>&&>(t);
}

template <typename T>
constexpr T&& forward(forg::RemoveReference_T<T>&& t) noexcept {
    return static_cast<T&&>(t);
}

template <class T>
constexpr T&& forward(forg::RemoveReference_T<T>& t) noexcept {
    return static_cast<T&&>(t);
}

template <typename T>
constexpr void moveSwap(T& first, T& second) noexcept {
    T temp(isMovable(first));
    first  = isMovable(second);
    second = isMovable(temp);
}

template <typename T>
constexpr void swap(T& first, T& second) noexcept {
    auto temp = first;
    first     = second;
    second    = temp;
}

template <typename Iter>
constexpr void iterSwap(Iter first, Iter second) {
    forg::moveSwap(*first, *second);
}

template <class T, size_t N>
constexpr void swap(T (&a)[N], T (&b)[N]) noexcept {
    auto tmp_a = a, tmp_b = b;
    for (size_t i = 0; i < N; i++) {
        iterSwap(tmp_a++, tmp_b++);
    }
}

template <class T, class U = T>
constexpr T exchange(T& obj, U&& new_value) {
    auto old_val = isMovable(obj);
    obj          = forward<U>(new_value);
    return old_val;
}

template <typename Value>
typename RemoveReference<Value>::type&& move(Value&& arg) noexcept {
    return static_cast<typename RemoveReference<Value>::type&&>(arg);
}

}  // namespace forg

#endif  // !UTILITY_HPP
