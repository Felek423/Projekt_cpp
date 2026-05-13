#include "game.h"
#include <cmath> 

void Game::GenerateMap() {
    dungeonMap.clear();
    currentX = 0; currentY = 0;
    int cx = 0, cy = 0;
    dungeonMap[{cx, cy}].generated = true;

    int maxDist = 0;
    int bossX = 0, bossY = 0;

    for (int i = 0; i < 3; i++) {
        int dir = GetRandomValue(0, 3);
        int px = cx, py = cy;
        if (dir == 0) cy -= 1;
        else if (dir == 1) cy += 1;
        else if (dir == 2) cx -= 1;
        else if (dir == 3) cx += 1;

        dungeonMap[{px, py}].generated = true;
        dungeonMap[{cx, cy}].generated = true;
        if (dir == 0) { dungeonMap[{px, py}].hasTop = true; dungeonMap[{cx, cy}].hasBottom = true; }
        if (dir == 1) { dungeonMap[{px, py}].hasBottom = true; dungeonMap[{cx, cy}].hasTop = true; }
        if (dir == 2) { dungeonMap[{px, py}].hasLeft = true; dungeonMap[{cx, cy}].hasRight = true; }
        if (dir == 3) { dungeonMap[{px, py}].hasRight = true; dungeonMap[{cx, cy}].hasLeft = true; }

        // Skarbiec
        if (i == 2) {
            dungeonMap[{cx, cy}].type = 1;
        }

        // Szukamy najdalszego pokoju na Bossa
        int dist = abs(cx) + abs(cy);
        if (dist >= maxDist && dungeonMap[{cx, cy}].type != 1 && (cx != 0 || cy != 0)) {
            maxDist = dist;
            bossX = cx;
            bossY = cy;
        }
    }
    
    // zapisanie bossa na mapie
    dungeonMap[{bossX, bossY}].type = 3;
}

void Game::CheckRoomTransitions() {
    RoomData& currentRoom = dungeonMap[{currentX, currentY}];
    bool changedRoom = false;

    if (enemies.empty()) {
        if (!currentRoom.cleared) {
            currentRoom.cleared = true;
            clearedRoomsCount++;
            if (clearedRoomsCount % 3 == 0 && !currentRoom.heartSpawned){
                pickups.push_back({{roomX + roomWidth / 2.0f, roomY + roomHeight / 2.0f}, 1, true});
                currentRoom.heartSpawned = true;
            }
        }

        Rectangle playerRec = { playerPos.x - playerSize.x, playerPos.y - playerSize.y, playerSize.x * 2.0f, playerSize.y * 2.0f };
        if (currentRoom.hasRight && CheckCollisionRecs(playerRec, rightDoor) && playerPos.x > rightDoor.x + 20) {
            currentRoom.pickups = pickups; currentRoom.enemies = enemies; currentX += 1; playerPos.x = roomX + playerSize.x + 20; changedRoom = true;
        }
        else if (currentRoom.hasLeft && CheckCollisionRecs(playerRec, leftDoor) && playerPos.x < leftDoor.x + leftDoor.width - 20) {
            currentRoom.pickups = pickups; currentRoom.enemies = enemies; currentX -= 1; playerPos.x = roomX + roomWidth - playerSize.x - 20; changedRoom = true;
        }
        else if (currentRoom.hasTop && CheckCollisionRecs(playerRec, topDoor) && playerPos.y < topDoor.y + topDoor.height - 20) {
            currentRoom.pickups = pickups; currentRoom.enemies = enemies; currentY -= 1; playerPos.y = roomY + roomHeight - playerSize.y - 20; changedRoom = true;
        }
        else if (currentRoom.hasBottom && CheckCollisionRecs(playerRec, bottomDoor) && playerPos.y > bottomDoor.y + 20) {
            currentRoom.pickups = pickups; currentRoom.enemies = enemies; currentY += 1; playerPos.y = roomY + playerSize.y + 20; changedRoom = true;
        }
    } 

    if (changedRoom) {
        bullets.clear();
        RoomData& nextRoom = dungeonMap[{currentX, currentY}];
        pickups = nextRoom.pickups;

        if (!nextRoom.cleared && nextRoom.enemies.empty()) {
            obstacles.clear();
            enemies.clear();
            
            if (nextRoom.type == 1) {
                nextRoom.cleared = true;
                pickups.push_back({{roomX + roomWidth / 2.0f, roomY + roomHeight / 2.0f}, 2, true});
            } 
            else if (nextRoom.type == 3) {
                // POKOJ BOSSA 
                float bx = roomX + roomWidth / 2.0f;
                float by = roomY + roomHeight / 2.0f;
                enemies.push_back({{bx, by}, 50.0f, 60.0f, 4, 3.0f, 60, 0, 0});
                nextRoom.enemies = enemies;
            }
            else {
                // Zwykly pokoj
                float size = 100.0f; float margin = 50.0f;
                obstacles.push_back({roomX + margin, roomY + margin, size, size});
                obstacles.push_back({roomX + roomWidth - margin - size, roomY + margin, size, size});
                obstacles.push_back({roomX + margin, roomY + roomHeight - margin - size, size, size});
                obstacles.push_back({roomX + roomWidth - margin - size, roomY + roomHeight - margin - size, size, size});
                
             
                
                int enemyCount = GetRandomValue(3, 4);
                for(int i = 0; i < enemyCount; i++){
                    float ex = GetRandomValue(roomX + margin + size, roomX + roomWidth - margin - size);
                    float ey = GetRandomValue(roomY + margin + size, roomY + roomHeight - margin - size);
                    int type = GetRandomValue(1, 3);
                    float speed = GetRandomValue(60, 120); 
                    int hp = GetRandomValue(3, 6); 

                    enemies.push_back({{ex, ey}, speed, 35.0f, type, 2.0f, hp, 0, 0, 0, 0.0f, 0});
                }
            
                nextRoom.obstacles = obstacles;
                nextRoom.enemies = enemies;
            }
        } else {
            obstacles = nextRoom.obstacles;
            enemies = nextRoom.enemies;
        }
    }
}