#include "game.h"
#include <cmath> 
#include <vector>

void Game::GenerateMap() {
    dungeonMap.clear();
    currentX = 0; currentY = 0;

    // Pokój startowy
    dungeonMap[{0, 0}].generated = true;

    const int numRooms = 5;
    std::vector<std::pair<int, int>> roomCoords;
    roomCoords.push_back({0, 0});

    // Pętla generacji
    while (dungeonMap.size() < numRooms) {
        // Wybierz losowy istniejący pokój, od którego zaczniemy tworzyć odgałęzienie
        int randIndex = GetRandomValue(0, roomCoords.size() - 1);
        int px = roomCoords[randIndex].first;
        int py = roomCoords[randIndex].second;

        // Spróbuj znaleźć pustego sąsiada
        int attempts = 0;
        bool createdRoom = false;
        while (attempts < 10 && !createdRoom) {
            attempts++;
            int dir = GetRandomValue(0, 3);
            int cx = px, cy = py;

            if (dir == 0) cy -= 1;      // Góra
            else if (dir == 1) cy += 1; // Dół
            else if (dir == 2) cx -= 1; // Lewo
            else if (dir == 3) cx += 1; // Prawo

            // Jeśli pokój nie istnieje, stwórz go
            if (dungeonMap.find({cx, cy}) == dungeonMap.end()) {
                dungeonMap[{cx, cy}].generated = true;
                roomCoords.push_back({cx, cy});

                if (dir == 0) { dungeonMap[{px, py}].hasTop = true; dungeonMap[{cx, cy}].hasBottom = true; }
                if (dir == 1) { dungeonMap[{px, py}].hasBottom = true; dungeonMap[{cx, cy}].hasTop = true; }
                if (dir == 2) { dungeonMap[{px, py}].hasLeft = true; dungeonMap[{cx, cy}].hasRight = true; }
                if (dir == 3) { dungeonMap[{px, py}].hasRight = true; dungeonMap[{cx, cy}].hasLeft = true; }
                
                createdRoom = true;
            }
        }
    }

    // Znajdź najdalszy pokój na Bossa
    int maxDist = -1;
    int bossX = 0, bossY = 0;
    for (const auto& coords : roomCoords) {
        if (coords.first == 0 && coords.second == 0) continue; // Boss nie może być w pokoju startowym

        int dist = abs(coords.first) + abs(coords.second);
        if (dist > maxDist) {
            maxDist = dist;
            bossX = coords.first;
            bossY = coords.second;
        }
    }
    
    // zapisanie bossa na mapie
    dungeonMap[{bossX, bossY}].type = 3; // Pokój Bossa

    // Umieść pokój ze skarbem
    if (dungeonMap.size() > 2) {
        int treasureX, treasureY;
        do {
            int randIndex = GetRandomValue(0, roomCoords.size() - 1);
            treasureX = roomCoords[randIndex].first;
            treasureY = roomCoords[randIndex].second;
        } while ((treasureX == 0 && treasureY == 0) || (treasureX == bossX && treasureY == bossY));
        
        dungeonMap[{treasureX, treasureY}].type = 1; // Pokój ze skarbem
    }
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
                enemies.push_back({{bx, by}, 50.0f, 60.0f, 4, 3.0f, 5, 0, 0}); 
                nextRoom.enemies = enemies;
            }
            else {
                // Zwykly pokoj
                if (!roomLayouts.empty()) {
                    int layoutIdx = GetRandomValue(0, roomLayouts.size() - 1); // losowanie struktury
                    const auto& layout = roomLayouts[layoutIdx];
                    for (int r = 0; r < 9 && r < layout.size(); ++r) {
                        for (int c = 0; c < 14 && c < layout[r].size(); ++c) {
                            float cellX = roomX + c * 100.0f;
                            float cellY = roomY + r * 100.0f;
                            if (layout[r][c] == 'X') {
                                obstacles.push_back({cellX, cellY, 100.0f, 100.0f});
                            } else if (layout[r][c] == '.') {
                                int type = GetRandomValue(1, 3);
                                float speed = GetRandomValue(60, 90);
                                int hp = GetRandomValue(3, 5);
                                enemies.push_back({{cellX + 50.0f, cellY + 50.0f}, speed, 50.0f, type, 2.0f, hp, 0, 0, 0, 0.0f, 0});
                            }
                        }
                    }
                } else {
                    // Awaryjne generowanie pokoju, jeśli plik z układami nie istnieje 
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

                        enemies.push_back({{ex, ey}, speed, 50.0f, type, 2.0f, hp, 0, 0, 0, 0.0f, 0});
                    }
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