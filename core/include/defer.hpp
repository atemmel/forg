#pragma once

namespace forg::impl {

template <typename F>
struct Defer {
    F f;
    Defer(F f) : f(f) {
    }
    ~Defer() {
        f();
    }
};

template <typename F>
Defer<F> deferCall(F f) {
    return Defer<F>(f);
}
};  // namespace forg::impl

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define defer(code) \
    auto DEFER_3(_defer_) = forg::impl::deferCall([&]() { code; })
