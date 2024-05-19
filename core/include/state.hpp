#pragma once

#include <cstdlib>

#include "defer.hpp"
#include "uniquePtr.hpp"

namespace forg {

struct State {
    virtual ~State()              = default;
    virtual auto update() -> bool = 0;
    virtual auto draw() -> void   = 0;
};

namespace state {

auto init(const char* windowName) -> void;
auto deinit() -> void;
auto doUpdate() -> bool;
auto doDraw() -> void;
auto isRunning() -> bool;

auto quit() -> void;
auto change(State* state) -> void;
auto push(State* state) -> void;
auto pop() -> void;

template <typename StateAncestor>
auto run(const char* windowName) -> int {
    init(windowName);
    defer(deinit());

    UniquePtr<State> state = makeUnique<StateAncestor>();
    push(state.get());

    while (isRunning()) {
        if (!doUpdate() || !isRunning()) {
            break;
        }
        doDraw();
    }

    return EXIT_SUCCESS;
}

}  // namespace state

}  // namespace forg
