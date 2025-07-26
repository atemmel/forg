#include "arena.hpp"

#include "safety.hpp"

constexpr size_t LEAST_POSSIBLE_CHUNK_SIZE = 1024 * 1024;

namespace forg {

struct Buffer {
    char* ptr;
    size_t size;
    size_t capacity;
};

struct Arena::Node {
    Buffer buffer = {
        .ptr      = nullptr,
        .size     = 0,
        .capacity = 0,
    };
    Arena::Node* next = nullptr;

    auto reset() -> void {
        buffer.size = 0;
    }

    auto free_size() -> size_t {
        return buffer.capacity - buffer.size;
    }

    auto claim_space(size_t n) -> char* {
        assert(n <= free_size());
        auto ptr = buffer.ptr + buffer.size;
        buffer.size += n;
        return ptr;
    }
};

Arena::Arena(mem::AllocatorInterface* parent_allocator)
    : parent_allocator(parent_allocator) {
}

auto Arena::alloc(size_t n) -> void* {
    if (!node) {
        node = new_node(n);
        return node->claim_space(n);
    }

    auto prev = node;
    for (auto ptr = node; ptr; ptr = ptr->next) {
        if (ptr->free_size() >= n) {
            return ptr->claim_space(n);
        }
        prev = ptr;
    }

    auto tail = prev->next = new_node(n);
    return tail->claim_space(n);
}

auto Arena::free(void* ptr) -> void {
    // Don't do anything
}

auto Arena::reset() -> void {
    for (auto ptr = node; ptr; ptr = ptr->next) {
        ptr->reset();
    }
}

auto Arena::free_all() -> void {
    auto ptr = node;
    while (ptr) {
        parent_allocator->free(ptr->buffer.ptr);
        auto old = ptr;
        ptr      = ptr->next;
        parent_allocator->free(old);
    }

    node = nullptr;
}

auto Arena::new_node(size_t n) -> Node* {
    auto buffer_size = max(LEAST_POSSIBLE_CHUNK_SIZE, n + n / 2);
    auto node        = parent_allocator->create<Node>();

    *node = {
        .buffer =
            {
                .ptr      = (char*)parent_allocator->alloc(n),
                .size     = 0,
                .capacity = n,
            },
        .next = nullptr,
    };
    return node;
}

}  // namespace forg
