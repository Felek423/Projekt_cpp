#include "raylib.h"

int main(){
    InitWindow(800, 600, "Lochy");
    SetTargetFPS(30);

    //gracz
    Vector2 playerPos = {400, 300}; //pozycja gracza
    float playerSpeed = 200.0f; //szybkość gracza
    float playerSize = 20.0f; //rozmiar gracza

    while(!WindowShouldClose()){


        if(IsKeyDown(KEY_W)) playerPos.y -= playerSpeed * GetFrameTime(); //ruch w górę
        if(IsKeyDown(KEY_S)) playerPos.y += playerSpeed * GetFrameTime(); // w dol
        if(IsKeyDown(KEY_A)) playerPos.x -= playerSpeed * GetFrameTime(); // w lewo
        if(IsKeyDown(KEY_D)) playerPos.x += playerSpeed * GetFrameTime(); // w prawo

        BeginDrawing();

        DrawCircleV(playerPos, playerSize, BLUE); //rysowanie gracza
        ClearBackground(GRAY);
        EndDrawing();
    }
}
