#include "game.h"
#include <cmath>

void Game::UpdatePlayerMovement() {
    Vector2 oldPos = playerPos;
    isMoving = false;

    // Oś Y (Góra / Dół) - playerDir 1 to góra, 0 to dół
    if(IsKeyDown(KEY_W)) { playerPos.y -= playerSpeed * GetFrameTime(); isMoving = true; playerDir = 1; flipX = false; } 
    if(IsKeyDown(KEY_S)) { playerPos.y += playerSpeed * GetFrameTime(); isMoving = true; playerDir = 0; flipX = false; } 
    for(Rectangle rocks : obstacles){
        if(CheckCollisionCircleRec(playerPos, playerSize, rocks)){ playerPos.y = oldPos.y; }
    }

    // Oś X - playerDir 2 to lewo, dla prawego robimy flipX = true
    if(IsKeyDown(KEY_A)) { playerPos.x -= playerSpeed * GetFrameTime(); isMoving = true; playerDir = 2; flipX = false; } 
    if(IsKeyDown(KEY_D)) { playerPos.x += playerSpeed * GetFrameTime(); isMoving = true; playerDir = 2; flipX = true; }  
    for(Rectangle rocks : obstacles){
        if(CheckCollisionCircleRec(playerPos, playerSize, rocks)){ playerPos.x = oldPos.x; }
    }

    // Odmierzanie klatek animacji
    if (isMoving) {
        frameTimer += GetFrameTime();
        if (frameTimer >= frameSpeed) {
            frameTimer = 0.0f; 
            currentFrame++;
            if (currentFrame >= maxFrames) currentFrame = 0; 
        }
    } else {
        currentFrame = 0;
        frameTimer = 0.0f;
    }

    RoomData& currentRoom = dungeonMap[{currentX, currentY}];
    // ograniczenie ruchu gracza i sprawdzanie wyjsc
    float minX = roomX + playerSize;
    float maxX = roomX + roomWidth - playerSize;
    float minY = roomY + playerSize;
    float maxY = roomY + roomHeight - playerSize;

    if (enemies.empty()) {
        if (currentRoom.hasLeft && playerPos.y > leftDoor.y && playerPos.y < leftDoor.y + leftDoor.height) minX = roomX - playerSize;
        if (currentRoom.hasRight && playerPos.y > rightDoor.y && playerPos.y < rightDoor.y + rightDoor.height) maxX = roomX + roomWidth + playerSize;
        if (currentRoom.hasTop && playerPos.x > topDoor.x && playerPos.x < topDoor.x + topDoor.width) minY = roomY - playerSize;
        if (currentRoom.hasBottom && playerPos.x > bottomDoor.x && playerPos.x < bottomDoor.x + bottomDoor.width) maxY = roomY + roomHeight + playerSize;
    }

    if (playerPos.x < minX) playerPos.x = minX;
    if (playerPos.x > maxX) playerPos.x = maxX;
    if (playerPos.y < minY) playerPos.y = minY;
    if (playerPos.y > maxY) playerPos.y = maxY;
}

void Game::UpdatePickups() {
    // logika zbierania przedmiotow 
    for(int i = pickups.size() - 1; i >= 0; i--){
        if(CheckCollisionCircles(playerPos, playerSize, pickups[i].position, 15.0f)){
            if (pickups[i].type == 1 && playerHp < 6){
                playerHp += 1;
                pickups.erase(pickups.begin() + i);
            }
            else if (pickups[i].type == 2) {
                playerAttackSpeed -= 0.20f;
                if (playerAttackSpeed < 0.1f) playerAttackSpeed = 0.1f;
                pickups.erase(pickups.begin() + i);
            }
        }
    }
}

void Game::UpdatePlayerShooting() {
    // strzelanie
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
}

void Game::UpdateEnemies() {
    // ruch wrogow (podazanie za graczem + kolizje ze skałami)
    for(int i = 0; i < enemies.size(); i++) {
        float dx = playerPos.x - enemies[i].position.x;
        float dy = playerPos.y - enemies[i].position.y;
        float length = sqrt(dx*dx + dy*dy);
        
        // --- NOWE: Uniwersalna animacja i ustalanie kierunku ---
        if (fabsf(dx) > fabsf(dy)) {
            enemies[i].directionRow = (dx > 0) ? 3 : 2; // Prawo : Lewo
        } else {
            enemies[i].directionRow = (dy > 0) ? 0 : 1; // Dół : Góra
        }

        enemies[i].frameTimer += GetFrameTime();
        if (enemies[i].frameTimer >= 0.15f) { 
            enemies[i].frameTimer = 0.0f; 
            enemies[i].currentFrame++;    
            if (enemies[i].currentFrame >= 4) enemies[i].currentFrame = 0; 
        }

        if(length > 0) {
            dx = dx / length;
            dy = dy / length;
        }

        // obrażenia od kontaktu z przeciwnikiem
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

        // Zwykły ruch w stronę gracza (Typ 4 - Boss stoi w miejscu)
        float oldEnemyX = enemies[i].position.x;
        if (enemies[i].type != 4 && length > playerSize + enemies[i].size) {
            enemies[i].position.x += dx * enemies[i].speed * GetFrameTime();
        }               
        for(Rectangle rocks : obstacles){
            if(CheckCollisionCircleRec(enemies[i].position, enemies[i].size, rocks)){
                enemies[i].position.x = oldEnemyX;
            }
        }

        float oldEnemyY = enemies[i].position.y;
        if (enemies[i].type != 4 && length > playerSize + enemies[i].size) {
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
            float pushX = enemies[i].position.x - enemies[j].position.x;
            float pushY = enemies[i].position.y - enemies[j].position.y;
            float distance = sqrt(pushX*pushX + pushY*pushY);
            float margin = 25.0f; 
            float desiredDistance = enemies[i].size + enemies[j].size + margin;
            if(distance > 0.0f && distance < desiredDistance){
                float overlap = desiredDistance - distance;
                pushX /= distance;
                pushY /= distance;
                enemies[i].position.x += pushX * overlap * 0.5f;
                enemies[i].position.y += pushY * overlap * 0.5f;
            }
        }

        // strzelanie wrogow
        if (enemies[i].type == 4) {
            // logika ataku Bossa (typ 4)
            if (enemies[i].burstBulletsLeft > 0) {
                // strzelanie w serii
                enemies[i].burstInterval -= GetFrameTime();
                if (enemies[i].burstInterval <= 0.0f) {
                    float eSpeed = 450.0f; // pociski bossa lecą nieco szybciej
                    bullets.push_back({enemies[i].position, {dx, dy}, eSpeed, true, 0.0f});
                    enemies[i].burstBulletsLeft--;
                    enemies[i].burstInterval = 0.1f; // mala przerwa zeby powstal piekny "strumien"
                }
            } else {
                // oczekiwanie na kolejny atak
                enemies[i].shootTimer -= GetFrameTime();
                if (enemies[i].shootTimer <= 0.0f) {
                    enemies[i].shootTimer = 2.5f; // przeladowanie 3 sekundy
                    enemies[i].burstBulletsLeft = 80; // strzaly na cel
                    enemies[i].burstInterval = 0.0f; // pierwszy strzal serii startuje od razu
                }
            }
        } else {
            // logika zwyklych wrogow (krzyz, skos, wycelowany)
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
    }
}

void Game::UpdateBullets() {
    // ruch pocisku i niszczenie po uderzeniu 
    for(int i = bullets.size() - 1; i >= 0; i--){
        float step = GetFrameTime() * bullets[i].speed;
        bullets[i].position.x += bullets[i].direction.x * GetFrameTime() * bullets[i].speed;
        bullets[i].position.y += bullets[i].direction.y * GetFrameTime() * bullets[i].speed;
        bullets[i].attackRange += step;
        bool hitSomething = false;

        if(!bullets[i].isEnemy && bullets[i].attackRange > 300.0f){
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
                            if(enemies[j].type == 4){
                                isVictory = true;
                                isGameOver = true;
                            }
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