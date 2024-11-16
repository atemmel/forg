#pragma once

#include <cstdint>
#include <cstdio>

#include "string.hpp"
#include "types.hpp"
#include "utility.hpp"

namespace forg {

// must be forward declared
auto fprintType(File* file, bool value) -> void;
auto fprintType(File* file, char value) -> void;
auto fprintType(File* file, char* str) -> void;
auto fprintType(File* file, const char* str) -> void;
auto fprintType(File* file, double value) -> void;
auto fprintType(File* file, float value) -> void;
auto fprintType(File* file, int value) -> void;
auto fprintType(File* file, int64_t value) -> void;
auto fprintType(File* file, unsigned char value) -> void;
auto fprintType(File* file, usize value) -> void;
auto fprintType(File* file, const String& value) -> void;

auto fprint(File* file) -> void;

template <typename Param, typename... Params>
auto fprint(File* file, const Param& param, Params... params) -> void {
    fprintType(file, param);
    fprintf(file, " ");
    fprint(file, forward<Params>(params)...);
}

template <typename Param, typename... Params>
auto fprintln(File* file, const Param& param, Params... params) -> void {
    fprint(file, param, forward<Params>(params)...);
    fprintf(file, "\n");
}

template <typename Param, typename... Params>
auto print(const Param& param, Params... params) -> void {
    fprint(stdout, param, forward<Params>(params)...);
}

template <typename Param, typename... Params>
auto println(const Param& param, Params... params) -> void {
    fprintln(stdout, param, forward<Params>(params)...);
}

template <typename Param, typename... Params>
auto errprint(const Param& param, Params... params) -> void {
    fprint(stderr, param, forward<Params>(params)...);
}

template <typename Param, typename... Params>
auto errprintln(const Param& param, Params... params) -> void {
    fprintln(stderr, param, forward<Params>(params)...);
}

}  // namespace forg
