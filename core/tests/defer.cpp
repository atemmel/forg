#include "defer.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("defer test") {
    int i = 0;

    {
        defer(i++);
        defer({
            i++;
            REQUIRE(i == 1);
        });
        REQUIRE(i == 0);
    }

    REQUIRE(i == 2);
}
