#include "raylib.h"

int main(){
    InitWindow(800, 600, "Lochy");
    SetTargetFPS(30);

    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(GRAY);
        EndDrawing();
    }
}
