#include "print.hpp"

namespace forg {

auto fprintType(File* file, const char* str) -> void {
    if (str == nullptr) {
        fprintf(file, "null str");
    } else {
        fprintf(file, "%s", str);
    }
}

auto fprintType(File* file, char* str) -> void {
    if (str == nullptr) {
        fprintf(file, "null str");
    } else {
        fprintf(file, "%s", str);
    }
}

auto fprintType(File* file, char value) -> void {
    fprintf(file, "%c", value);
}

auto fprintType(File* file, unsigned char value) -> void {
    fprintf(file, "%c", value);
}

auto fprintType(File* file, int value) -> void {
    fprintf(file, "%d", value);
}

auto fprintType(File* file, int64_t value) -> void {
    fprintf(file, "%ld", value);
}

auto fprintType(File* file, size_t value) -> void {
    fprintf(file, "%lu", value);
}

auto fprintType(File* file, float value) -> void {
    fprintf(file, "%f", value);
}

auto fprintType(File* file, double value) -> void {
    fprintf(file, "%f", value);
}

auto fprintType(File* file, bool value) -> void {
    fprintf(file, value ? "true" : "false");
}

auto fprint(File* file) -> void{};

auto fprintln(File* file) -> void {
    fprintf(file, "\n");
}

}  // namespace forg
