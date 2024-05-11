#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include "range.hpp"

namespace forg {

template <typename InIt, typename OutIt, typename Func>
inline OutIt transform(const InIt first, const InIt last, OutIt dest,
                       Func func) {
    for (auto tempFirst = first; tempFirst != last; tempFirst++, dest++) {
        *dest = func(*tempFirst);
    }
    return dest;
}

template <typename T>
bool cycleDetect(const T list) {
    if (list->begin() == list->end()) {
        return false;
    }

    auto tortoise = list->begin();
    auto hare     = list->begin()->next;

    while (hare != tortoise && hare != nullptr) {
        tortoise = tortoise->next;
        hare     = hare->next->next;
    }
    if (hare == tortoise) {
        return true;
    }
    return false;
}

template <class T>
T& search(forg::Slice<T> slice, T value) {
    for (auto It = slice.begin(); It != slice.end(); It++) {
        if (*It == value) {
            return *It;
        }
    }
    return *slice.end();
}

usize strlen(const char* str);

int strcmp(const char* lhs, const char* rhs);

}  // namespace forg

#endif  // !ALGORITHMS_HPP
