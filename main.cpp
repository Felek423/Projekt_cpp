#include "raylib.h"
#include <vector>
#include <cmath>
using namespace std;    

struct bullet{
    Vector2 position;
    Vector2 direction;
    float speed;
    bool isEnemy; //rozroznienie miedzy pociskami gracza i przeciwnikow 
    float attackRange = 10.0f;
};

struct enemy{
    Vector2 position;
    float speed;
    float size; 
    int type;   //rodzaj strzelania 1(na krzyz) 2(na skos) 3(wycelowany w gracza)
    float shootTimer; 
    int hp;
};

struct pickup {
    Vector2 position;
    int type; // 1 = polowa serca
    bool active;
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
    float playerSize = 30.0f; //rozmiar gracza
    int playerHp = 6;
    float invincibilityTimer = 0.0f;
    float playerAttackSpeed = 0.6f;
    float playerShootTimer = 0.0f;

    screenWidth = GetMonitorWidth(GetCurrentMonitor());
    screenHeight = GetMonitorHeight(GetCurrentMonitor());    
    ToggleFullscreen();


    // definiowanie pokoju
        float roomWidth = 1400;
        float roomHeight = 900;
        float roomX = (screenWidth - roomWidth) / 2.0f;
        float roomY = (screenHeight - roomHeight) / 2.0f;

    //definiowanie przeszkody
    vector<Rectangle> obstacles = {
             {roomX + 200, roomY + 150, 100 , 100},
             {roomX + 700, roomY + 300, 100 , 100}, 
             {roomX + 1000, roomY + 500, 100 , 300}
    };

    vector<bullet> bullets;
    float bulletSpeed = 150.0f;
    float bulletSize = 10.0f;

    //definiowanie przeciwnikow 
    vector<enemy> enemies= {
        {{roomX + 700, roomY + 200}, 150.0f, 20.0f, 1, 2.0f, 5}
    };

    //przedmioty
    vector<pickup> pickups;
    bool heartSpawnInThisRoom = false;


    //drzwi na prawej scianie
    Rectangle rightDoor = {roomX + roomWidth - 50, roomY + roomHeight / 2.0f - 60, 50, 120};
    int roomCount = 1;

    bool isPaused = false;
    bool isGameOver = false;

    while(!WindowShouldClose()){

        //obsługa wlaczania i wylaczania menu pauzy po wcisnieciu ESC
        if(IsKeyPressed(KEY_ESCAPE)){
            isPaused = !isPaused;
        }
        if(!isPaused && !isGameOver){
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

        //oganiczenie ruchu gracza
        if (playerPos.x - playerSize <= roomX) playerPos.x = roomX + playerSize + 5;
        if (playerPos.y - playerSize <= roomY) playerPos.y = roomY + playerSize + 5;
        if (playerPos.y + playerSize >= roomY + roomHeight) playerPos.y = roomY + roomHeight - playerSize - 5;

        //  logika prawej ściany i drzwi
        if (enemies.empty()) {
            //logika spawnowania serca jako nagrody
            if (roomCount % 3 == 0 && !heartSpawnInThisRoom){
                pickups.push_back({{roomX + roomWidth / 2.0f, roomY + roomHeight / 2.0f}, 1, true});
                heartSpawnInThisRoom = true;
                }
            // pokoj jest pusty i drzwi sie otwieraja
            if (CheckCollisionCircleRec(playerPos, playerSize, rightDoor)) {            
                playerPos.x = roomX + playerSize + 20;
                playerPos.y = roomY + roomHeight / 2.0f;
                pickups.clear();// usuwanie niezebranych przedmiotow
                heartSpawnInThisRoom = false; //reset watosci dla nowego pokoju
                bullets.clear();
                roomCount++;
                obstacles.clear();

                //nowe przeszkody
                float size = 100.0f;
                float margin = 50.0f;
                obstacles.push_back({roomX + margin, roomY + margin, size, size});
                obstacles.push_back({roomX + roomWidth - margin - size, roomY + margin, size, size});
                obstacles.push_back({roomX + margin, roomY + roomHeight - margin - size, size, size});
                obstacles.push_back({roomX + roomWidth - margin - size, roomY + roomHeight - margin - size, size, size});
                // losowa liczba przeciwnikow (od 3 do 6)
                int enemyCount = GetRandomValue(3, 6);
                for(int i = 0; i < enemyCount; i++){
                    //losowanie pozycji X tylko po prawej stronie,Y dowolne
                    float ex = GetRandomValue(roomX + roomWidth / 2.0f, roomX + roomWidth - margin);
                    float ey = GetRandomValue(roomY + margin, roomY + roomHeight - margin);

                    int type = GetRandomValue(1, 3); // typ strzelania
                    float speed = GetRandomValue(60, 120); // predkosc poruszania
                    int hp = GetRandomValue(3, 6); // hp 

                    enemies.push_back({{ex, ey}, speed, 20.0f, type, 2.0f, hp});
                }                
            } 


            else if (playerPos.x + playerSize >= roomX + roomWidth) {
                playerPos.x = roomX + roomWidth - playerSize - 5;
            }
        } else {
            // gdy przeciwnicy wciaz zyja drzwi sa zamkniete
            if (playerPos.x + playerSize >= roomX + roomWidth) {
                playerPos.x = roomX + roomWidth - playerSize - 5;
            }
        }
        //logika zbierania przedmiotow 
        for(int i = pickups.size() - 1; i >= 0; i--){
            if(CheckCollisionCircles(playerPos, playerSize, pickups[i].position, 15.0f)){
                if (playerHp < 6){
                    playerHp += 1;
                    if (playerHp > 6){
                        playerHp = 6;
                    }
                    pickups.erase(pickups.begin() + i);
                }
            }
        }
        //strzelanie
        if(playerShootTimer <= 0.0f){
            bool hasShot = false;


        if(IsKeyDown(KEY_UP)) { bullets.push_back({playerPos, {0, -1} , bulletSpeed, false, 10.0f}); hasShot = true; }
        else if(IsKeyDown(KEY_DOWN)) {bullets.push_back({playerPos, {0, 1} , bulletSpeed, false, 10.0f }); hasShot = true; }
        else if(IsKeyDown(KEY_LEFT)) {bullets.push_back({playerPos, {-1, 0} , bulletSpeed, false, 10.0f }); hasShot = true; }
        else if(IsKeyDown(KEY_RIGHT)) {bullets.push_back({playerPos, {1, 0} , bulletSpeed, false, 10.0f }); hasShot = true; }

        if(hasShot){
            playerShootTimer = playerAttackSpeed;
        }
    }
        
        //ruch wrogow (podazanie za graczem + kolizje ze skałami)
        for(int i = 0; i < enemies.size(); i++) {
            // uzywanie twierdzenia Pitagorasa, aby wyliczyć najkrótszą drogę w linii prostej od wroga do gracza.
            float dx = playerPos.x - enemies[i].position.x;
            float dy = playerPos.y - enemies[i].position.y;
            float length = sqrt(dx*dx + dy*dy); 
            
            if(length > 0) {
                dx = dx / length;
                dy = dy / length;
            }

            // Obrażenia od kontaktu z przeciwnikiem
            if (invincibilityTimer <= 0.0f) {
            if (length <= (playerSize + enemies[i].size)) {
                playerHp -= 1;
                invincibilityTimer = 1.0f; 
                
            if (playerHp <= 0) {
                isGameOver = true;
        }
    }
}

            // omijanie przeszkod
            for(Rectangle rocks : obstacles) {
                float closestX = fmaxf(rocks.x, fminf(enemies[i].position.x, rocks.x + rocks.width));
                float closestY = fmaxf(rocks.y, fminf(enemies[i].position.y, rocks.y + rocks.height));

                float diffX = enemies[i].position.x - closestX;
                float diffY = enemies[i].position.y - closestY;
                float distance = sqrt(diffX * diffX + diffY * diffY);

                float buffer = 50.0f; 

                if (distance < buffer) {
                    if (distance == 0) { enemies[i].position.y -= 1; continue; }

                    float dirX = diffX / distance;
                    float dirY = diffY / distance;

                    float pushForce = (buffer - distance) * 5.0f;
                    enemies[i].position.x += dirX * pushForce * GetFrameTime();
                    enemies[i].position.y += dirY * pushForce * GetFrameTime();

                    float dotProduct = dx * (-dirX) + dy * (-dirY); 

                    if (dotProduct > 0.3f) { 
                        float slideX = -dirY; 
                        float slideY = dirX;
                        if (slideX * dx + slideY * dy < 0) {
                            slideX = -slideX;
                            slideY = -slideY;
                        }

                        //wektor od srodka do przeszkodS
                        float rockCenterX = rocks.x + rocks.width / 2.0f;
                        float rockCenterY = rocks.y + rocks.height / 2.0f;
                        float toPlayerX = playerPos.x - rockCenterX;
                        float toPlayerY = playerPos.y - rockCenterY;

                        toPlayerX += 0.1f;
                        toPlayerY += 0.1f;

                        if (slideX * toPlayerX + slideY * toPlayerY < 0) {
                            slideX = -slideX;
                            slideY = -slideY;
                        }

                        float slideSpeed = enemies[i].speed; 
                        enemies[i].position.x += slideX * slideSpeed * GetFrameTime();
                        enemies[i].position.y += slideY * slideSpeed * GetFrameTime();
                    }
                }
            }

            // os Y Niezależne sprawdzanie osi Y
            float oldEnemyX = enemies[i].position.x; 
            if (length > playerSize + enemies[i].size) {
              enemies[i].position.x += dx * enemies[i].speed * GetFrameTime();
                }               
            for(Rectangle rocks : obstacles){
                if(CheckCollisionCircleRec(enemies[i].position, enemies[i].size, rocks)){
                    enemies[i].position.x = oldEnemyX; 
                }
            }

            float oldEnemyY = enemies[i].position.y; 
            if (length > playerSize + enemies[i].size) {
               enemies[i].position.y += dy * enemies[i].speed * GetFrameTime();
                }
                
            for(Rectangle rocks : obstacles){
                if(CheckCollisionCircleRec(enemies[i].position, enemies[i].size, rocks)){
                    enemies[i].position.y = oldEnemyY; 
                }   
            }

            // odpychanie sie wrogow od siebie
            for(int j = 0; j < enemies.size(); j++) {
                if(i == j) continue; 
                if(CheckCollisionCircles(enemies[i].position, enemies[i].size, enemies[j].position, enemies[j].size)){
                    float pushX = enemies[i].position.x - enemies[j].position.x;
                    float pushY = enemies[i].position.y - enemies[j].position.y;
                    float distance = sqrt(pushX*pushX + pushY*pushY);
                    if(distance > 0.0f){
                        float overlap = (enemies[i].size + enemies[j].size) - distance;
                        pushX /= distance;
                        pushY /= distance;
                        enemies[i].position.x += pushX * overlap * 2.0f;
                        enemies[i].position.y += pushY * overlap * 2.0f;
                    }
                }
            }

            enemies[i].shootTimer -= GetFrameTime();
            
            if (enemies[i].shootTimer <= 0.0f){
                enemies[i].shootTimer = 2.0f;
                float eSpeed = 200.0f;
                if(enemies[i].type == 1){
                    bullets.push_back({enemies[i].position, {1, 0}, eSpeed, true, 0.0f});
                    bullets.push_back({enemies[i].position, {-1, 0}, eSpeed, true, 0.0f});
                    bullets.push_back({enemies[i].position, {0, 1}, eSpeed, true, 0.0f});
                    bullets.push_back({enemies[i].position, {0, -1}, eSpeed, true, 0.0f});
                }
                else if(enemies[i].type == 2){
                    bullets.push_back({enemies[i].position, {-0.70f, -0.70f}, eSpeed, true, 0.0f});
                    bullets.push_back({enemies[i].position, {0.70f, -0.70f}, eSpeed, true, 0.0f});
                    bullets.push_back({enemies[i].position, {0.70f, 0.70f}, eSpeed, true, 0.0f});
                    bullets.push_back({enemies[i].position, {-0.70f, 0.70f}, eSpeed, true, 0.0f});
                }
                else if(enemies[i].type == 3){
                    bullets.push_back({enemies[i].position, {dx, dy}, eSpeed, true, 0.0f});
                }
            }
        }

        //ruch pocisku
        for(int i = bullets.size() - 1; i >= 0; i--){
            float step = GetFrameTime() * bullets[i].speed;

            bullets[i].position.x += bullets[i].direction.x * GetFrameTime() * bullets[i].speed;
            bullets[i].position.y += bullets[i].direction.y * GetFrameTime() * bullets[i].speed;
            bullets[i].attackRange += step;
            bool hitSomething = false;

            if(!bullets[i].isEnemy && bullets[i].attackRange > 200.0f){
                hitSomething = true;
            }
            
            if(bullets[i].position.x < roomX || bullets[i].position.x > roomX + roomWidth ||
               bullets[i].position.y < roomY || bullets[i].position.y > roomY + roomHeight){
                hitSomething = true;
            }

            if(!hitSomething){
                for(Rectangle rocks : obstacles){
                    if(CheckCollisionCircleRec(bullets[i].position, bulletSize, rocks)){
                        hitSomething = true;
                        break;
                    }
                }
            }

            if(!hitSomething) {
                if (bullets[i].isEnemy) {
                    if(invincibilityTimer <= 0.0f){
                        if (CheckCollisionCircles(bullets[i].position, bulletSize, playerPos, playerSize)) {
                            hitSomething = true;
                            playerHp -= 1;  
                            invincibilityTimer = 1.0f;
                            if(playerHp <= 0){
                                isGameOver = true;
                            }
                        }
                    }    
                } else {
                    for(int j = enemies.size() - 1; j >= 0; j--) {
                        if(CheckCollisionCircles(bullets[i].position, bulletSize, enemies[j].position, enemies[j].size)) {
                            hitSomething = true;
                            enemies[j].hp -= 1;
                            if(enemies[j].hp <= 0){
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
        else if (isPaused){
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
        else if (isGameOver) {
            Rectangle btnRestart = { (float)screenWidth/2 - 100, (float)screenHeight/2 - 60, 200, 50 };
            Rectangle btnQuit = { (float)screenWidth/2 - 100, (float)screenHeight/2 + 10, 200, 50 };
            Vector2 mousePos = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, btnRestart)) {
                    playerPos = {400, 300};
                    playerHp = 6;
                    invincibilityTimer = 0.0f;
                    playerShootTimer = 0.0f;
                    roomCount = 1;
                    bullets.clear();
                    enemies.clear();
                    enemies.push_back({{roomX + 700, roomY + 200}, 150.0f, 20.0f, 1, 2.0f, 5});
                    obstacles = {
                        {roomX + 200, roomY + 150, 100 , 100},
                        {roomX + 700, roomY + 300, 100 , 100}, 
                        {roomX + 1000, roomY + 500, 100 , 300}
                    };
                    isGameOver = false;
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
        Rectangle roomRect = {roomX, roomY, roomWidth, roomHeight}; 
        DrawRectangleLinesEx(roomRect, 5, DARKGREEN); 
        
        //rysowanie drzwi
        if(enemies.empty()){
            DrawRectangleRec(rightDoor, BLACK); 
            DrawText(">", rightDoor.x + 15, rightDoor.y + 40, 40, WHITE); 
        }
        
        DrawText(TextFormat("POKOJ: %d", roomCount), roomX + 10, roomY + 10, 20, DARKGREEN);
        
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
            DrawRectangleRounded(rocks, 0.2f, 10, BLACK); 
            DrawRectangleRoundedLinesEx(rocks, 0.2f, 10, 5.0f, DARKGRAY); 
        }

        //rysowanie pociskow
        for(bullet b: bullets){
            if (b.isEnemy) DrawCircleV(b.position, bulletSize, YELLOW);
            else DrawCircleV(b.position, bulletSize, RED);
        }

        //rysowanie hp gracza
        int maxHearts = 3;
        for (int i = 0; i < maxHearts; i++){
            int hx = 50 + i * 60; 
            int hy = 50;
            if(playerHp >= (i*2) + 2) DrawRectangle(hx, hy, 40, 40, RED); 
            else if (playerHp >= (i*2) + 1){
                DrawRectangle(hx, hy, 20, 40, RED); 
                DrawRectangleLines(hx, hy, 40, 40, RED); 
            }
            else DrawRectangleLines(hx, hy, 40, 40, RED); 
        }
        //rysowanie pickupow na ziemi  
        for(pickup p : pickups){
            if (p.type = 1)
            {
                float pulse = sin(GetTime() * 5.0f) * 2.0f;

                DrawCircleV(p.position, 12.0f + pulse, RED);
                DrawCircleV({p.position.x - 6, p.position.y - 4}, 6.0f + pulse/2, RED);
                DrawCircleV({p.position.x + 6, p.position.y - 4}, 6.0f + pulse/2, RED);
            }
            
            
        }
        //ekran pauzy
        if (isPaused) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
            DrawText("PAUZA", screenWidth/2 - MeasureText("PAUZA", 40)/2, screenHeight/2 - 150, 40, WHITE);

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

        if (isGameOver) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.6f));
            DrawText("KONIEC GRY", screenWidth/2 - MeasureText("KONIEC GRY", 40)/2, screenHeight/2 - 150, 40, WHITE);

            Rectangle btnRestart = { (float)screenWidth/2 - 100, (float)screenHeight/2 - 60, 200, 50 };
            Rectangle btnQuit = { (float)screenWidth/2 - 100, (float)screenHeight/2 + 10, 200, 50 };
            Vector2 mousePos = GetMousePosition();

            Color restartColor = CheckCollisionPointRec(mousePos, btnRestart) ? LIGHTGRAY : GRAY;
            Color quitColor = CheckCollisionPointRec(mousePos, btnQuit) ? LIGHTGRAY : GRAY;

            DrawRectangleRec(btnRestart, restartColor);
            DrawRectangleLinesEx(btnRestart, 2, BLACK);
            DrawText("Od nowa", btnRestart.x + 60, btnRestart.y + 15, 20, BLACK);

            DrawRectangleRec(btnQuit, quitColor);
            DrawRectangleLinesEx(btnQuit, 2, BLACK);
            DrawText("Wyjscie", btnQuit.x + 60, btnQuit.y + 15, 20, BLACK);
        }
        EndDrawing();
    } 
    return 0;
}