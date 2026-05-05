#include "game.h"
#include <cmath>

void Game::DrawRoom() {
    DrawRectangle(roomX, roomY, roomWidth, roomHeight, GREEN); 
    Rectangle roomRect = {roomX, roomY, roomWidth, roomHeight};
    DrawRectangleLinesEx(roomRect, 5, DARKGREEN); 
    
    // rysowanie drzwi jezeli nie ma wrogow
    if(enemies.empty()){
        RoomData& drawRoom = dungeonMap[{currentX, currentY}];
        if (drawRoom.hasTop) { DrawRectangleRec(topDoor, BLACK); DrawText("^", topDoor.x + 50, topDoor.y + 10, 40, WHITE); }
        if (drawRoom.hasBottom) { DrawRectangleRec(bottomDoor, BLACK); DrawText("v", bottomDoor.x + 50, bottomDoor.y + 10, 40, WHITE); }
        if (drawRoom.hasLeft) { DrawRectangleRec(leftDoor, BLACK); DrawText("<", leftDoor.x + 10, leftDoor.y + 40, 40, WHITE); }
        if (drawRoom.hasRight) { DrawRectangleRec(rightDoor, BLACK); DrawText(">", rightDoor.x + 15, rightDoor.y + 40, 40, WHITE); }
    }
    
    DrawText(TextFormat("ZDOBYTE POKOJE: %d", clearedRoomsCount - 1), roomX + 10, roomY + 10, 20, DARKGREEN);
}

void Game::DrawEntities() {
    // rysowanie gracza z efektem niesmiertelnosci
    Color playerColor = BLUE;
    if(invincibilityTimer > 0.0f){
        if((int)(invincibilityTimer * 10) % 2 == 0){
            playerColor = RED;
        }  
    }
    DrawCircleV(playerPos, playerSize, playerColor);

    // rysowanie wrogow z kolorami typow strzalow
    for(enemy e : enemies){
        Color enemyColor = PURPLE;
        if(e.type == 1) enemyColor = RED;
        else if(e.type == 2) enemyColor = ORANGE;
        else if(e.type == 3) enemyColor = YELLOW;
        DrawCircleV(e.position, e.size, enemyColor);
    }

    // rysowanie przeszkod
    for(Rectangle rocks : obstacles){
        DrawRectangleRounded(rocks, 0.2f, 10, BLACK);
        DrawRectangleRoundedLinesEx(rocks, 0.2f, 10, 5.0f, DARKGRAY); 
    }

    // rysowanie pociskow
    for(bullet b: bullets){
        if (b.isEnemy) DrawCircleV(b.position, bulletSize, YELLOW);
        else DrawCircleV(b.position, bulletSize, RED);
    }

    // rysowanie pickupow na ziemi  
    for(pickup p : pickups){
        if (p.type == 1) {
            float pulse = sin(GetTime() * 5.0f) * 2.0f;
            DrawCircleV(p.position, 12.0f + pulse, RED);
            DrawCircleV({p.position.x - 6, p.position.y - 4}, 6.0f + pulse/2, RED);
            DrawCircleV({p.position.x + 6, p.position.y - 4}, 6.0f + pulse/2, RED);
        }
        else if (p.type == 2) {
            float hover = sin(GetTime() * 4.0f) * 5.0f;
            DrawRectangle(p.position.x - 10, p.position.y - 10 + hover, 20, 20, GOLD);
            DrawRectangleLines(p.position.x - 10, p.position.y - 10 + hover, 20, 20, ORANGE);
        }
    }
}

void Game::DrawUI() {
    // tlo i rysowanie minimapy
    DrawRectangle(screenWidth - 210, 10, 200, 200, Fade(BLACK, 0.5f));
    DrawRectangleLines(screenWidth - 210, 10, 200, 200, WHITE);

    for (auto const& [coords, room] : dungeonMap) {
        if (room.generated) {
            Color c = DARKGRAY;
            if (coords.first == currentX && coords.second == currentY) c = GREEN;
            else if (room.type == 1) c = GOLD; 
            else if (room.cleared) c = LIGHTGRAY;
            
            DrawRectangle(screenWidth - 110 + (coords.first - currentX) * 15, 
                          110 + (coords.second - currentY) * 15, 
                          13, 13, c);
        }
    }

    // rysowanie hp gracza (puste / polpelne / pelne serca)
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
}

void Game::DrawMenus() {
    // nakladka ekranu pauzy
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

    // nakladka ekranu konca gry
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

        // obsługa resetowania wewnatrz gry jezeli gracz zginie
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePos, btnRestart)) {
                playerPos = {400, 300};
                playerHp = 6;
                invincibilityTimer = 0.0f;
                playerShootTimer = 0.0f;
                playerAttackSpeed = 0.6f;
                
                bullets.clear();
                pickups.clear();
                enemies.clear();
                enemies.push_back({{roomX + 700, roomY + 200}, 150.0f, 20.0f, 1, 2.0f, 5});
                obstacles = {
                    {roomX + 200, roomY + 150, 100 , 100},
                    {roomX + 700, roomY + 300, 100 , 100}, 
                    {roomX + 1000, roomY + 500, 100 , 300}
                };
                clearedRoomsCount = 1;
                GenerateMap();
                dungeonMap[{0, 0}].obstacles = obstacles;
                dungeonMap[{0, 0}].enemies = enemies;
                isGameOver = false;
            }
        }
    }
}