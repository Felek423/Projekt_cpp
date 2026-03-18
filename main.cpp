#include "raylib.h"

int main(){
    int screenWidth = 800;
    int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Lochy");
    SetTargetFPS(30);

    //gracz
    Vector2 playerPos = {400, 300}; //pozycja gracza
    float playerSpeed = 200.0f; //szybkość gracza
    float playerSize = 20.0f; //rozmiar gracza
    screenWidth = GetMonitorWidth(GetCurrentMonitor());
    screenHeight = GetMonitorHeight(GetCurrentMonitor());    
    ToggleFullscreen();


    // definiowanie pokoju
        float roomWidth = 1000;
        float roomHeight = 600;
        float roomX = (screenWidth - roomWidth) / 2.0f;
        float roomY = (screenHeight - roomHeight) / 2.0f;

    while(!WindowShouldClose()){

        //poruszanie się gracza wsad
        if(IsKeyDown(KEY_W)) playerPos.y -= playerSpeed * GetFrameTime(); 
        if(IsKeyDown(KEY_S)) playerPos.y += playerSpeed * GetFrameTime(); 
        if(IsKeyDown(KEY_A)) playerPos.x -= playerSpeed * GetFrameTime(); 
        if(IsKeyDown(KEY_D)) playerPos.x += playerSpeed * GetFrameTime(); 

        //ograniczenie ruchu gracza do obszaru pokoju
        if (playerPos.x - playerSize <= roomX) playerPos.x = roomX + playerSize + 5;
        if (playerPos.x + playerSize >= roomX + roomWidth) playerPos.x = roomX + roomWidth - playerSize - 5;
        if (playerPos.y - playerSize <= roomY) playerPos.y = roomY + playerSize + 5;
        if (playerPos.y + playerSize >= roomY + roomHeight) playerPos.y = roomY + roomHeight - playerSize - 5;

        BeginDrawing();
        DrawRectangle(roomX, roomY, roomWidth, roomHeight, GREEN); //rysowanie pokoju
        Rectangle roomRect = {roomX, roomY, roomWidth, roomHeight}; //definiowanie prostokąta pokoju
        DrawRectangleLinesEx(roomRect, 5, DARKGREEN); //rysowanie obramowania pokoju
        DrawCircleV(playerPos, playerSize, BLUE); //rysowanie gracza
        ClearBackground(GRAY);
        EndDrawing();
    }
}
