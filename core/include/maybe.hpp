#pragma once

namespace forg {

template <typename T>
struct Maybe {
    Maybe() : presence(false) {
    }

    Maybe(const T& param) : value(param), presence(true) {
    }

    Maybe(T&& param) : value(param), presence(true) {
    }

    const T& get() const {
        return value;
    }

    T& get() {
        return value;
    }

    bool empty() const {
        return !presence;
    }

    bool present() const {
        return presence;
    }

private:
    T value;
    bool presence;
};

};  // namespace forg
