#include "raylib.h"

int main(){
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Lochy");
    ToggleFullscreen();
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

        // definiowanie pokoju
        float roomWidth = 600.0f;
        float roomHeight = 400.0f;
        float roomX = (screenWidth - roomWidth) / 2.0f;
        float roomY = (screenHeight - roomHeight) / 2.0f;

        //ograniczenie ruchu gracza do obszaru pokoju
        if (playerPos.x - playerSize <= roomX) playerPos.x = roomX + playerSize;
        if (playerPos.x + playerSize >= roomX + roomWidth) playerPos.x = roomX + roomWidth - playerSize;
        if (playerPos.y - playerSize <= roomY) playerPos.y = roomY + playerSize;
        if (playerPos.y + playerSize >= roomY + roomHeight) playerPos.y = roomY + roomHeight - playerSize;  

        BeginDrawing();
        DrawRectangle(roomX, roomY, roomWidth, roomHeight, GREEN); //rysowanie pokoju
        DrawCircleV(playerPos, playerSize, BLUE); //rysowanie gracza
        ClearBackground(GRAY);
        EndDrawing();
    }
}
