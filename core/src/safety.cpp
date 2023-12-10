#include "safety.hpp"

#include <stdio.h>
#include <stdlib.h>

#include "runtime.hpp"
#include "types.hpp"

auto assertCall(const char *file, const usize line, const char *function)
    -> void {
    char stacktrace[4096];
    runtime::dumpStackTrace(stacktrace, sizeof(stacktrace));
    fprintf(stderr,
            "Assertion failed in:\nfile:%s, line:%lu, "
            "function:%s\nstacktrace:\n\n%s",
            file, line, function, stacktrace);
    exit(EXIT_FAILURE);
}
