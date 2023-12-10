#pragma once

#include "types.hpp"

auto assertCall(const char *file, const usize line, const char *function)
    -> void;

#undef assert

#ifndef NDEBUG
#define assert(x)                                            \
    if (!(x)) {                                              \
        assertCall(__FILE__, __LINE__, __PRETTY_FUNCTION__); \
    }
#else
#define assert(x) (void(0))
#endif
