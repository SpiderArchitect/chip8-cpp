#include "raylib.h"

int main() {
    InitWindow(640, 320, "CHIP-8 Emulator"); // Standard Chip-8 scale (64x32 * 10)
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw a pixel representing a CHIP-8 sprite
        // (Scale 10x so we can see it)
        DrawRectangle(100, 100, 10, 10, GREEN); 
        
        EndDrawing();
    }
    CloseWindow();
    return 0;
}