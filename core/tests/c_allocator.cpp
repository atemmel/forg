#include <catch2/catch_all.hpp>

#include "memory.hpp"

using namespace forg::mem;

TEST_CASE("C Allocator test") {
    AllocatorInterface* allocator = &cAllocator;
    auto my_int_ptr               = allocator->create<int>();
    REQUIRE(my_int_ptr != 0);
    allocator->destroy(my_int_ptr);
}
