#include "state.hpp"

#include <raylib.h>

#include "array.hpp"
#include "defer.hpp"
#include "safety.hpp"

namespace forg {

namespace state {

static bool running = true;
static Array<State*> states;

auto init(const char* windowName, int width, int height) -> void {
    assert(states.empty());
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(width, height, windowName);
    SetTargetFPS(60);
}

auto deinit() -> void {
    while (states.empty()) {
        states.popBack();
    }
    CloseWindow();
}

auto doUpdate() -> bool {
    assert(!states.empty());
    // TODO: this should probably not be here...
    if (IsKeyPressed(KEY_ESCAPE)) {
        return false;
    }
    return states[states.size() - 1]->update();
}

auto doDraw() -> void {
    assert(!states.empty());
    BeginDrawing();
    defer(EndDrawing());
    states[states.size() - 1]->draw();
}

auto isRunning() -> bool {
    return !WindowShouldClose() && running;
}

auto quit() -> void {
    running = false;
}

auto change(State* state) -> void {
    assert(!states.empty());
    auto& back = states[states.size() - 1];
    back->~State();
    back = state;
}

auto push(State* state) -> void {
    states.append(state);
}

auto pop() -> void {
    assert(!states.empty());
    states.popBack();
    if (states.empty()) {
        quit();
    }
}

}  // namespace state

}  // namespace forg
