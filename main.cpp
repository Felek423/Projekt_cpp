#include "raylib.h"

int main(){
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Lochy");
    SetTargetFPS(30);

    //gracz
    Vector2 playerPos = {400, 300}; //pozycja gracza
    float playerSpeed = 200.0f; //szybkość gracza
    float playerSize = 20.0f; //rozmiar gracza

    while(!WindowShouldClose()){

        //poruszanie się gracza wsad
        if(IsKeyDown(KEY_W)) playerPos.y -= playerSpeed * GetFrameTime(); 
        if(IsKeyDown(KEY_S)) playerPos.y += playerSpeed * GetFrameTime(); 
        if(IsKeyDown(KEY_A)) playerPos.x -= playerSpeed * GetFrameTime(); 
        if(IsKeyDown(KEY_D)) playerPos.x += playerSpeed * GetFrameTime(); 

        //ograniczenie ruchu gracza do obszaru ekranu
        if(playerPos.x < playerSize) playerPos.x = playerSize;
        if(playerPos.x > screenWidth - playerSize) playerPos.x = screenWidth - playerSize; 
        if(playerPos.y < playerSize) playerPos.y = playerSize; 
        if(playerPos.y > screenHeight - playerSize) playerPos.y = screenHeight - playerSize; 

        BeginDrawing();

        DrawCircleV(playerPos, playerSize, BLUE); //rysowanie gracza
        ClearBackground(GRAY);
        EndDrawing();
    }
}
