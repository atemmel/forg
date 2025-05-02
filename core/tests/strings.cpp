#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "string.hpp"

TEST_CASE("fmt test") {
    auto str = forg::String::fmt("my str");

    REQUIRE("my str" == str);
}
