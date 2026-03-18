#include "raylib.h"
#include <vector>
using namespace std;    

struct bullet{
    Vector2 position;
    Vector2 direction;
    float speed;
};

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

    //definiowanie przeszkody
    vector<Rectangle> obstacles = {
             {roomX + 200, roomY + 150, 100 , 100},
             {roomX + 700, roomY + 300, 100 , 100}, 
    };

    vector<bullet> bullets;
    float bulletSpeed = 100.0f;
    float bulletSize = 10.0f;


    while(!WindowShouldClose()){

        Vector2 oldPos = playerPos;

        //poruszanie się gracza wsad
        if(IsKeyDown(KEY_W)) playerPos.y -= playerSpeed * GetFrameTime(); 
        if(IsKeyDown(KEY_S)) playerPos.y += playerSpeed * GetFrameTime(); 
        for(Rectangle rocks : obstacles){
            if(CheckCollisionCircleRec(playerPos, playerSize, rocks)){
                playerPos.y = oldPos.y; //przywrócenie poprzedniej pozycji gracza
            }
        }

        if(IsKeyDown(KEY_A)) playerPos.x -= playerSpeed * GetFrameTime(); 
        if(IsKeyDown(KEY_D)) playerPos.x += playerSpeed * GetFrameTime(); 

        for(Rectangle rocks : obstacles){
            if(CheckCollisionCircleRec(playerPos, playerSize, rocks)){
                playerPos.x = oldPos.x; 
            }
        }

        //ograniczenie ruchu gracza do obszaru pokoju
        if (playerPos.x - playerSize <= roomX) playerPos.x = roomX + playerSize + 5;
        if (playerPos.x + playerSize >= roomX + roomWidth) playerPos.x = roomX + roomWidth - playerSize - 5;
        if (playerPos.y - playerSize <= roomY) playerPos.y = roomY + playerSize + 5;
        if (playerPos.y + playerSize >= roomY + roomHeight) playerPos.y = roomY + roomHeight - playerSize - 5;

        //strzelanie
        if(IsKeyPressed(KEY_UP)) bullets.push_back({playerPos, {0, - bulletSpeed} , 5});
        if(IsKeyPressed(KEY_DOWN)) bullets.push_back({playerPos, {0, bulletSpeed} , 5});
        if(IsKeyPressed(KEY_LEFT)) bullets.push_back({playerPos, {-bulletSpeed, 0} , 5});
        if(IsKeyPressed(KEY_RIGHT)) bullets.push_back({playerPos, {bulletSpeed, 0} , 5});
        
        //ruch pocisku
        for(int i = bullets.size() - 1; i >= 0; i--){
            bullets[i].position.x += bullets[i].direction.x * GetFrameTime() * bullets[i].speed;
            bullets[i].position.y += bullets[i].direction.y * GetFrameTime() * bullets[i].speed;
            bool hitSomething = false;
            
            //kolizja pocisku z scianami pokoju   
            if(bullets[i].position.x < roomX || bullets[i].position.x > roomX + roomWidth ||
               bullets[i].position.y < roomY || bullets[i].position.y > roomY + roomHeight){
                hitSomething = true;
            }
            //kolizzja pociksu z przeszkodami
            if(!hitSomething){
                for(Rectangle rocks : obstacles){
                    if(CheckCollisionCircleRec(bullets[i].position, bulletSize, rocks)){
                        hitSomething = true;
                        break;
                    }
                    }
            }

            if(hitSomething){
                bullets.erase(bullets.begin() + i);
            }
        }

        BeginDrawing(); 
        ClearBackground(GRAY);
        DrawRectangle(roomX, roomY, roomWidth, roomHeight, GREEN); //rysowanie pokoju
        Rectangle roomRect = {roomX, roomY, roomWidth, roomHeight}; //definiowanie prostokąta pokoju
        DrawRectangleLinesEx(roomRect, 5, DARKGREEN); //rysowanie obramowania pokoju
        DrawCircleV(playerPos, playerSize, BLUE); //rysowanie gracza
        for(Rectangle rocks : obstacles){
            DrawRectangleRec(rocks, BLACK);
            DrawRectangleLinesEx(rocks, 5, DARKGRAY);
        }
        for(bullet b: bullets){
            DrawCircleV(b.position, bulletSize, RED);
        }
        EndDrawing();
    } 
}