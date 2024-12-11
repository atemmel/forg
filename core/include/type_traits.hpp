#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP

#include "types.hpp"

namespace forg {

template <typename T>
struct RemoveReference {
    typedef T Type;
};

template <typename T>
struct RemoveReference<T&> {
    typedef T Type;
};

template <typename T>
struct RemoveReference<T&&> {
    typedef T Type;
};

template <typename T>
using RemoveReference_T = typename RemoveReference<T>::Type;

template <typename T, typename D>
struct addPointer {
    typedef T* Type;
};
template <typename T, typename D>
using addPointer_T = typename addPointer<T, D>::Type;

template <class T>
struct RemoveExtent {
    typedef T Type;
};

template <class T>
struct RemoveExtent<T[]> {
    typedef T Type;
};

template <class T, usize N>
struct RemoveExtent<T[N]> {
    typedef T Type;
};

template <typename T>
using RemoveExtent_T = typename RemoveExtent<T>::Type;

template <bool B, class T = void>
struct EnableIf {};

template <class T>
struct EnableIf<true, T> {
    typedef T Type;
};

template <bool B, class T = void>
using EnableIf_T = typename EnableIf<B, T>::Type;

template <class T, T val>
struct IntegralConstant {
    typedef T value_type;
    typedef IntegralConstant type;

    constexpr static T value = val;

    constexpr operator value_type() const noexcept {
        return value;
    }

    constexpr value_type operator()() const noexcept {
        return value;
    }
};

template <bool B>
using BoolConstant = IntegralConstant<bool, B>;

using TrueType  = BoolConstant<true>;
using FalseType = BoolConstant<false>;

template <class T>
struct IsIntegral : FalseType {};
template <>
struct IsIntegral<int> : TrueType {};
template <>
struct IsIntegral<long long> : TrueType {};

template <class T>
inline constexpr bool IsIntegral_v = IsIntegral<T>::value;

template <class T>
struct IsArray : FalseType {};

template <class T>
struct IsArray<T[]> : TrueType {};

template <class T, usize N>
struct IsArray<T[N]> : TrueType {};

template <class T>
inline constexpr bool IsArray_T = IsArray<T>::value;

template <bool B, class T, class F>
struct conditional {
    using type = T;
};

template <class T, class F>
struct conditional<false, T, F> {
    using type = F;
};

template <class T>
struct is_const : FalseType {};
template <class T>
struct is_const<const T> : TrueType {};

}  // namespace forg

#endif  // !TYPE_TRAITS_HPP
