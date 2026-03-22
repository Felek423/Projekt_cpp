#include "raylib.h"
#include <vector>
#include <cmath>
using namespace std;    

struct bullet{
    Vector2 position;
    Vector2 direction;
    float speed;
    bool isEnemy; //rozroznienie miedzy pociskami gracza i przeciwnikow 
};

struct enemy{
    Vector2 position;
    float speed;
    float size; 
    int type;   //rodzaj strzelania 1(na krzyz) 2(na skos) 3(wycelowany w gracza)
    float shootTimer; 
    int hp;
};

int main(){
    int screenWidth = 800;
    int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Lochy");
    SetExitKey(0);
    SetTargetFPS(30);

    //gracz
    Vector2 playerPos = {400, 300}; //pozycja gracza
    float playerSpeed = 200.0f; //szybkość gracza
    float playerSize = 20.0f; //rozmiar gracza
    int playerHp = 6;
    float invincibilityTimer = 0.0f;
    float playerAttackSpeed = 0.6f;
    float playerShootTimer = 0.0f;

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

    vector<enemy> enemies= {
        {{roomX + 700, roomY + 200}, 50.0f, 20.0f, 1, 2.0f, 5},
        {{roomX + 100, roomY + 400}, 50.0f, 20.0f, 2, 3.0f, 5},
        {{roomX + 500, roomY + 100}, 50.0f, 20.0f, 3, 4.0f, 5},
    };

    bool isPaused = false;

    while(!WindowShouldClose()){

        if(IsKeyPressed(KEY_ESCAPE)){
            isPaused = !isPaused;
        }
        if(!isPaused){
            if(invincibilityTimer > 0.0f){
                invincibilityTimer -= GetFrameTime();
            }

            if(playerShootTimer > 0.0f){
                playerShootTimer -= GetFrameTime();
            }
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
        if(playerShootTimer <= 0.0f){
            bool hasShot = false;


        if(IsKeyDown(KEY_UP)) { bullets.push_back({playerPos, {0, -1} , bulletSpeed, false}); hasShot = true; }
        else if(IsKeyDown(KEY_DOWN)) {bullets.push_back({playerPos, {0, 1} , bulletSpeed, false}); hasShot = true; }
        else if(IsKeyDown(KEY_LEFT)) {bullets.push_back({playerPos, {-1, 0} , bulletSpeed, false}); hasShot = true; }
        else if(IsKeyDown(KEY_RIGHT)) {bullets.push_back({playerPos, {1, 0} , bulletSpeed, false}); hasShot = true; }

        if(hasShot){
            playerShootTimer = playerAttackSpeed;
        }
    }
        
        //ruch wrogow (podazanie za graczem + kolizje ze skałami)
        for(int i = 0; i < enemies.size(); i++) {
            float dx = playerPos.x - enemies[i].position.x;
            float dy = playerPos.y - enemies[i].position.y;
            float length = sqrt(dx*dx + dy*dy); 
            
            if(length > 0) {
                dx = dx / length;
                dy = dy / length;
            }
            
            float oldEnemyX = enemies[i].position.x; // Zapisujemy starą pozycję X
            enemies[i].position.x += dx * enemies[i].speed * GetFrameTime();
            
            for(Rectangle rocks : obstacles){
                if(CheckCollisionCircleRec(enemies[i].position, enemies[i].size, rocks)){
                    enemies[i].position.x = oldEnemyX; // Cofamy ruch X, jeśli uderzył
                }
            }

            float oldEnemyY = enemies[i].position.y; 
            enemies[i].position.y += dy * enemies[i].speed * GetFrameTime();
            
            for(Rectangle rocks : obstacles){
                if(CheckCollisionCircleRec(enemies[i].position, enemies[i].size, rocks)){
                    enemies[i].position.y = oldEnemyY; 
                }   
            }
            enemies[i].shootTimer -= GetFrameTime();
            
            if (enemies[i].shootTimer <= 0.0f){
                enemies[i].shootTimer = 2.0f;
                float eSpeed = 200.0f;

                if(enemies[i].type == 1){
                    bullets.push_back({enemies[i].position, {1, 0}, eSpeed, true});
                    bullets.push_back({enemies[i].position, {-1, 0}, eSpeed, true});
                    bullets.push_back({enemies[i].position, {0, 1}, eSpeed, true});
                    bullets.push_back({enemies[i].position, {0, -1}, eSpeed, true});
                }
                else if(enemies[i].type == 2){
                    bullets.push_back({enemies[i].position, {-0.70f, -0.70f}, eSpeed, true});
                    bullets.push_back({enemies[i].position, {0.70f, -0.70f}, eSpeed, true});
                    bullets.push_back({enemies[i].position, {0.70f, 0.70f}, eSpeed, true});
                    bullets.push_back({enemies[i].position, {-0.70f, 0.70f}, eSpeed, true});
                }
                else if(enemies[i].type == 3){
                    bullets.push_back({enemies[i].position, {dx, dy}, eSpeed, true});
                }
            }
        }

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
            // Kolizja w zależności od właściciela pocisku
            if(!hitSomething) {
                if (bullets[i].isEnemy) {
                    if(invincibilityTimer <= 0.0f){
                    // Jeśli to pocisk przeciwnika, sprawdza kolizję z graczem
                    if (CheckCollisionCircles(bullets[i].position, bulletSize, playerPos, playerSize)) {
                        hitSomething = true;
                        playerHp -= 1;  
                        invincibilityTimer = 1.0f;
                        if(playerHp <= 0){
                            CloseWindow();
                            return 0;
                        }
                    }
                }    
                    
                } else {
                    // Jeśli to pocisk gracza, sprawdza kolizję z przeciwnikami
                    for(int j = enemies.size() - 1; j >= 0; j--) {
                        if(CheckCollisionCircles(bullets[i].position, bulletSize, enemies[j].position, enemies[j].size)) {
                            hitSomething = true;
                            enemies[j].hp -= 1;
                            if(enemies[j].hp <+ 0){
                                enemies.erase(enemies.begin() + j); 
                            }
                            break;
                        }
                    }
                }
            }

            if(hitSomething){
                bullets.erase(bullets.begin() + i);
            }
        }
        
        }
    else{
            Rectangle btnResume = { (float)screenWidth/2 - 100, (float)screenHeight/2 - 60, 200, 50 };
            Rectangle btnQuit = { (float)screenWidth/2 - 100, (float)screenHeight/2 + 10, 200, 50 };
            Vector2 mousePos = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, btnResume)) {
                    isPaused = false; 
                }
                if (CheckCollisionPointRec(mousePos, btnQuit)) {
                    CloseWindow(); 
                    return 0;      
                }
            }
        }

        BeginDrawing(); 
        ClearBackground(GRAY);
        DrawRectangle(roomX, roomY, roomWidth, roomHeight, GREEN); //rysowanie pokoju
        Rectangle roomRect = {roomX, roomY, roomWidth, roomHeight}; //definiowanie prostokąta pokoju
        DrawRectangleLinesEx(roomRect, 5, DARKGREEN); //rysowanie obramowania pokoju
        Color playerColor = BLUE;
        if(invincibilityTimer > 0.0f){
            if((int)(invincibilityTimer * 10) % 2 == 0){
                playerColor = RED;
            }  
        }
        DrawCircleV(playerPos, playerSize, playerColor); //rysowanie gracza
        //rysowanie wrogow
        for(enemy e : enemies){
            Color enemyColor = PURPLE;
            if(e.type == 1) enemyColor = RED;
            else if(e.type == 2) enemyColor = ORANGE;
            else if(e.type == 3) enemyColor = YELLOW;
            DrawCircleV(e.position, e.size, enemyColor);

        }
        //rysowanie przeszkod
        for(Rectangle rocks : obstacles){
            DrawRectangleRec(rocks, BLACK);
            DrawRectangleLinesEx(rocks, 5, DARKGRAY);
        }
        //rysowanie pociskow
        for(bullet b: bullets){
            if (b.isEnemy){
                DrawCircleV(b.position, bulletSize, YELLOW);
            }
            else{
                DrawCircleV(b.position, bulletSize, RED);
            }
        }
        //rysowanie hp gracza
        int maxHearts = 3;
        for (int i = 0; i < maxHearts; i++){
            int hx = 20 + i * 40; // odstep miedzy sercami
            int hy = 20;
            if(playerHp >= (i*2) + 2){
                DrawRectangle(hx, hy, 30, 30, RED); // pelne serce
            }
            else if (playerHp >= (i*2) + 1){
                DrawRectangle(hx, hy, 15, 30, RED); // czesc pelnego serca
                DrawRectangleLines(hx, hy, 30, 30, RED); // pusta ramka
            }
            else {
                DrawRectangleLines(hx, hy, 30, 30, RED); // puste serce
            }
        }
        if (isPaused) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
            
            const char* pauseText = "PAUZA";
            int textWidth = MeasureText(pauseText, 40);
            DrawText(pauseText, screenWidth/2 - textWidth/2, screenHeight/2 - 150, 40, WHITE);

            Rectangle btnResume = { (float)screenWidth/2 - 100, (float)screenHeight/2 - 60, 200, 50 };
            Rectangle btnQuit = { (float)screenWidth/2 - 100, (float)screenHeight/2 + 10, 200, 50 };
            Vector2 mousePos = GetMousePosition();

            Color resumeColor = CheckCollisionPointRec(mousePos, btnResume) ? LIGHTGRAY : GRAY;
            Color quitColor = CheckCollisionPointRec(mousePos, btnQuit) ? LIGHTGRAY : GRAY;

            DrawRectangleRec(btnResume, resumeColor);
            DrawRectangleLinesEx(btnResume, 2, BLACK);
            DrawText("Kontynuuj", btnResume.x + 50, btnResume.y + 15, 20, BLACK);

            DrawRectangleRec(btnQuit, quitColor);
            DrawRectangleLinesEx(btnQuit, 2, BLACK);
            DrawText("Wyjscie", btnQuit.x + 60, btnQuit.y + 15, 20, BLACK);
        }
        EndDrawing();
    } 
    return 0;
}


