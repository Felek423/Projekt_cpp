#pragma once
#include "types.h"
#include <map>
#include <utility>

class Game {
public:
    Game(int screenWidth, int screenHeight);
    ~Game(); //sprzatanie pamieci graficznej
    
    void Update();
    void Draw();
    
    bool isPaused;
    bool isGameOver;
    bool isVictory;

private:

    Texture2D playerSprite;
    int currentFrame;
    int maxFrames;
    float frameTimer;
    float frameSpeed;
    bool isMoving;
    int playerDir; // 0 dol, 1 gora, 2 lewo, prawos
    bool flipX; // odbicie lustrzane

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