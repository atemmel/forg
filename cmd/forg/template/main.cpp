#include <forg/core.hpp>
#include <forg/state.hpp>
#include <raylib.h>

using namespace forg;

struct MyState : public State {

  auto update() -> bool override { return true; }

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
  MyState mystate{};
  println("Starting...");
  return state::run("My game", &mystate);
}
