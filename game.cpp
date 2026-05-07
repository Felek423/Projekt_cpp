#include "game.h"
#include <cmath>

Game::Game(int sw, int sh) {
    screenWidth = sw;
    screenHeight = sh;

    playerPos = {400, 300};
    playerSpeed = 200.0f;
    playerSize = 30.0f;
    playerHp = 6;
    invincibilityTimer = 0.0f;
    playerAttackSpeed = 0.6f;
    playerShootTimer = 0.0f;

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
        {roomX + 1000, roomY + 500, 100 , 300}
    };

    enemies.push_back({{roomX + 700, roomY + 200}, 150.0f, 20.0f, 1, 2.0f, 5, 0, 0});

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