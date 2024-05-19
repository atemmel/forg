#include <raylib.h>

#include <forg/core.hpp>
#include <forg/state.hpp>

using namespace forg;

struct MyState : public State {
    auto update() -> bool override {
        return true;
    }

    auto draw() -> void override {
        ClearBackground(Color{
            .r = 35,
            .g = 35,
            .b = 45,
            .a = 255,
        });

        DrawText("Running", 30, 30, 20, WHITE);
    }
};

auto main() -> int {
    println("Starting...");
    return state::run<MyState>("My game");
}
