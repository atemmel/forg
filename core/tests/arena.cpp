#include "arena.hpp"

#include <catch2/catch_all.hpp>

#include "memory.hpp"

using namespace forg;
using namespace forg::mem;

TEST_CASE("Arena test") {
    Arena my_arena(&cAllocator);

    auto my_int_ptr = my_arena.create<int>();
    REQUIRE(my_int_ptr != 0);

    my_arena.destroy(my_int_ptr);  // no-op

    auto my_second_int_ptr = my_arena.create<int>();
    REQUIRE(my_second_int_ptr != 0);

    auto my_huge_array = my_arena.alloc(1024 * 1024 * 5);
    REQUIRE(my_huge_array != 0);

    my_arena.free_all();
}
