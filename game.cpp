#include "game.h"
#include <cmath>

Game::Game(int sw, int sh) {
    screenWidth = sw;
    screenHeight = sh;

    playerPos = {400, 300};
    playerSpeed = 200.0f;
    playerSize = {30.0f, 50.0f}; // szerokosc / wysokosc gracza
    playerHp = 6;
    invincibilityTimer = 0.0f;
    playerAttackSpeed = 0.6f;
    playerShootTimer = 0.0f;

    // ladowanie grafiki  
    
    //bohATERA
    Image image = LoadImage("mag.png"); 
    playerSprite = LoadTextureFromImage(image); 
    UnloadImage(image);

    Image floorImage = LoadImage("podlogaaaa.png");
    floorSprite = LoadTextureFromImage(floorImage);
    UnloadImage(floorImage);

    Image rockImage = LoadImage("kamien.png");
    rockSprite = LoadTextureFromImage(rockImage);
    UnloadImage(rockImage);
    rockScale = {4.0f, 2.5f}; // szerokosc / wysokosc skali kamienia
    for(int i = 0; i < 5; i++) {
        hasEnemySprite[i] = false;
    }

    //wrog typ 3
    Image imageT3 = LoadImage("enemy.png"); 
    enemySprites[3] = LoadTextureFromImage(imageT3); 
    hasEnemySprite[3] = true; // Zaznaczamy, że typ 3 ma grafikę
    UnloadImage(imageT3);

    // Ustawienia animacji 4x4
    maxFrames = 4;           
    currentFrame = 0;
    frameTimer = 0.0f;
    frameSpeed = 0.15f;      
    isMoving = false;
    playerDir = 0;           
    flipX = false;




    roomWidth = 1400;
    roomHeight = 900;
    roomX = (screenWidth - roomWidth) / 2.0f;
    roomY = (screenHeight - roomHeight) / 2.0f;

    bulletSpeed = 150.0f;
    bulletSize = 10.0f;

    topDoor = {roomX + roomWidth / 2.0f - 60, roomY, 120, 50};
    bottomDoor = {roomX + roomWidth / 2.0f - 60, roomY + roomHeight - 50, 120, 50};
    leftDoor = {roomX, roomY + roomHeight / 2.0f - 60, 50, 120};
    rightDoor = {roomX + roomWidth - 50, roomY + roomHeight / 2.0f - 60, 50, 120};

    isPaused = false;
    isGameOver = false;
    isVictory = false;
    clearedRoomsCount = 1;

    obstacles = {
        {roomX + 200, roomY + 150, 100 , 100},
        {roomX + 700, roomY + 300, 100 , 100}, 
        {roomX + 1000, roomY + 500, 100 , 100},
        {roomX + 1000, roomY + 650, 100 , 100}
    };

    enemies.push_back({{roomX + 700, roomY + 200}, 150.0f, 20.0f, 3, 2.0f, 5, 0, 0});

    GenerateMap();
    dungeonMap[{0, 0}].obstacles = obstacles;
    dungeonMap[{0, 0}].enemies = enemies;
}

void Game::Update() {
    if(invincibilityTimer > 0.0f) invincibilityTimer -= GetFrameTime();
    if(playerShootTimer > 0.0f) playerShootTimer -= GetFrameTime();

    // tu uruchamiamy pociete kawalki kodu
    UpdatePlayerMovement();
    CheckRoomTransitions();
    UpdatePickups();
    UpdatePlayerShooting();
    UpdateEnemies();
    UpdateBullets();
}

void Game::Draw() {
    // wyrysowanie wszystkiego po kolei
    DrawRoom();
    DrawEntities();
    DrawUI();
    DrawMenus();
}

Game::~Game() {
    UnloadTexture(playerSprite);
    UnloadTexture(floorSprite);
    UnloadTexture(rockSprite);
    UnloadTexture(enemyType3Sprite); 
    for(int i = 1; i <= 4; i++){
        if(hasEnemySprite[i]) UnloadTexture(enemySprites[i]);
    }
}