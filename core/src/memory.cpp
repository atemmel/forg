#include <cstdlib>

#include "memory.hpp"

namespace forg::mem {

auto CAllocator::alloc(size_t n) -> void* {
    return ::malloc(n);
}
auto CAllocator::free(void* ptr) -> void {
    return ::free(ptr);
}

}  // namespace forg::mem
