#include "runtime.hpp"

#include <stdlib.h>

#include "types.hpp"

#ifndef _WIN32
#include <cxxabi.h>
#include <execinfo.h>
#else
#include <DbgHelp.h>
#include <Windows.h>
#include <process.h>
#endif

// discard the dump calls
constexpr static int DISCARD_PRE = 2;

#ifdef _WIN32
// discard __scrt functions
constexpr static int DISCARD_POST = 6;
#else
// discard first two calls (_start, __libc_start_main)
constexpr static int DISCARD_POST = 2;
#endif

namespace runtime {

static auto concatStr(const char *str, char *dest, const char *destEnd)
    -> char * {
    if (dest >= destEnd) {
        return dest;
    }

    char c;
    while ((c = *str++) && dest != destEnd) {
        *dest++ = c;
    }

    if (dest != destEnd) {
        *dest++ = '\n';
    }
    *dest = 0;
    return dest;
}

static auto seekUntil(char *buf, char delim) -> char * {
    for (int i = 0; buf[i]; i++) {
        if (buf[i] == delim) {
            return buf + i;
        }
    }
    return nullptr;
}

#ifdef _WIN32
auto dumpWin(char *buf, int size) -> void {
    buf[0]                   = 0;
    constexpr static int max = 64;
    void *stack[max];
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, nullptr, TRUE);
    WORD numberOfFrames = CaptureStackBackTrace(0, max, stack, nullptr);
    SYMBOL_INFO *symbol =
        (SYMBOL_INFO *)malloc(sizeof(SYMBOL_INFO) + (max - 1) * sizeof(TCHAR));
    symbol->MaxNameLen   = max;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    DWORD displacement;
    IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *)malloc(sizeof(IMAGEHLP_LINE64));
    line->SizeOfStruct    = sizeof(IMAGEHLP_LINE64);
    for (int i = DISCARD_PRE; i < numberOfFrames - DISCARD_POST; i++) {
        DWORD64 address = (DWORD64)(stack[i]);
        SymFromAddr(process, address, nullptr, symbol);
        SymGetLineFromAddr64(process, address, &displacement, line);
        buf = concatStr(symbol->Name, buf, buf + size);
    }
}
#else
auto dumpUnix(char *buf, usize size) -> void {
    buf[0]                   = 0;
    constexpr static int max = 64;
    void *frames[max];
    int n = backtrace(frames, max);

    // If backtrace call failed
    if (n == 0) {
        return;
    }

    // Fetch backtrace symbols
    char **symbols = backtrace_symbols(frames, n);
    char *f1;
    char *f2;
    char *demangled;
    int status;

    for (int i = 0 + DISCARD_PRE; i < n - DISCARD_POST; i++) {
        // Reset status
        status = -5;

        // Seek until function signature end and begin
        f1 = seekUntil(symbols[i], '(');
        f2 = seekUntil(symbols[i], '+');

        // If seek operation was successful
        if (f1 && f2) {
            *f2 = '\0';
            ++f1;
            // Request demangled function name
            demangled = abi::__cxa_demangle(f1, nullptr, nullptr, &status);
        }

        // If demangling was successful
        if (status == 0) {
            buf = concatStr(demangled, buf, buf + size);
            free(demangled);
        } else if (f1 && f2) {
            buf = concatStr(f1, buf, buf + size);
        } else {
            buf = concatStr(symbols[i], buf, buf + size);
        }
    }

    free(symbols);
}
#endif

auto dumpStackTrace(char *buf, usize size) -> void {
#ifdef _WIN32
    dumpWin(buf, size);
#else
    dumpUnix(buf, size);
#endif
}

}  // namespace runtime
