#include <cstdlib>
#include <forg/string.hpp>
#include <iostream>
#include <raylib.h>

auto main() -> int {
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(800, 450, "Window");
  SetTargetFPS(60);

  forg::String str = "Hello";
  std::cout << str.data() << '\n';

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_ESCAPE)) {
      break;
    }

    BeginDrawing();
    ClearBackground(Color{
        .r = 35,
        .g = 35,
        .b = 45,
        .a = 255,
    });

    DrawText("Running", 30, 30, 20, WHITE);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}
