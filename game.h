#pragma once
#include "types.h"
#include <map>
#include <utility>
#include <string>
#include <vector>

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

    Texture2D floorSprite;
    Texture2D heartSprite;
    float uiHeartScale;
    float pickupHeartScale;

    Texture2D itemSprite;
    float itemScale;

    Texture2D rockSprite;
    Vector2 rockScale;

    Texture2D playerBulletSprite;
    Texture2D enemyBulletSprite;
    float bulletScale;

    Texture2D enemySprites[5];
    bool hasEnemySprite[5];
    Texture2D enemyType3Sprite; 
    int eType3MaxFrames = 4;          // 4 kolumny
    float eType3FrameSpeed = 0.15f;   // Szybkość animacji

    // funkcje logiczne, ktore zostaly rozbite na glowna petle
    void GenerateMap();
    void UpdatePlayerMovement();
    void CheckRoomTransitions();
    void UpdatePickups();
    void UpdatePlayerShooting();
    void UpdateEnemies();
    void UpdateBullets();
    
    void LoadRoomLayouts();

    // funkcje rysujące
    void DrawRoom();
    void DrawEntities();
    void DrawUI();
    void DrawMenus();

    // gracz
    Vector2 playerPos;
    float playerSpeed;
    Vector2 playerSize;
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
    std::vector<std::vector<std::string>> roomLayouts;
    
    int screenWidth;
    int screenHeight;
};