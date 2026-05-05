#pragma once
#include "types.h"
#include <map>
#include <utility>

class Game {
public:
    Game(int screenWidth, int screenHeight);
    
    void Update();
    void Draw();
    
    bool isPaused;
    bool isGameOver;

private:
    // funkcje logiczne, na które rozbiliśmy główną pętlę
    void GenerateMap();
    void UpdatePlayerMovement();
    void CheckRoomTransitions();
    void UpdatePickups();
    void UpdatePlayerShooting();
    void UpdateEnemies();
    void UpdateBullets();

    // funkcje rysujące
    void DrawRoom();
    void DrawEntities();
    void DrawUI();
    void DrawMenus();

    // gracz
    Vector2 playerPos;
    float playerSpeed;
    float playerSize;
    int playerHp;
    float invincibilityTimer;
    float playerAttackSpeed;
    float playerShootTimer;

    // definiowanie pokoju i drzwi
    float roomWidth;
    float roomHeight;
    float roomX;
    float roomY;
    Rectangle topDoor;
    Rectangle bottomDoor;
    Rectangle leftDoor;
    Rectangle rightDoor;

    // obiekty w grze
    std::vector<Rectangle> obstacles;
    std::vector<bullet> bullets;
    float bulletSpeed;
    float bulletSize;
    std::vector<enemy> enemies;
    std::vector<pickup> pickups;

    // mapa lochow
    std::map<std::pair<int, int>, RoomData> dungeonMap;
    int currentX;
    int currentY;
    int clearedRoomsCount;
    
    int screenWidth;
    int screenHeight;
};