#pragma once

#include "memory.hpp"

namespace forg {

struct Arena : public mem::AllocatorInterface {
    struct Node;

    mem::AllocatorInterface* parent_allocator;
    Node* node = nullptr;

    Arena(mem::AllocatorInterface* parent_allocator);

    auto alloc(size_t n) -> void* override;
    auto free(void* ptr) -> void override;

    auto reset() -> void;
    auto free_all() -> void;

private:
    auto new_node(size_t) -> Node*;
};

}  // namespace forg
