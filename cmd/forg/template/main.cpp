#include <cstdlib>
#include <forg/core.hpp>
#include <raylib.h>

using namespace forg;

auto main() -> int {
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(800, 450, "Window");
  defer(CloseWindow());
  SetTargetFPS(60);

  println("Starting...");

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_ESCAPE)) {
      break;
    }

    BeginDrawing();
    defer(EndDrawing());
    ClearBackground(Color{
        .r = 35,
        .g = 35,
        .b = 45,
        .a = 255,
    });

    DrawText("Running", 30, 30, 20, WHITE);
  }
  return EXIT_SUCCESS;
}
