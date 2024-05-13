#include <catch2/catch_all.hpp>

#include "core.hpp"

TEST_CASE("defer test") {
    forg::println(1, 3.14, "Hi hello", forg::String("My string"));
    forg::errprintln(1, 3.14, "Hi hello", forg::String("My string"));
}
