#include <cstdlib>
#include <raylib.h>

auto main() -> int {
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(800, 450, "Window");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_ESCAPE)) {
      break;
    }

    BeginDrawing();
    ClearBackground(Color{
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255,
    });

    DrawText("Running", 30, 30, 20, WHITE);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}
