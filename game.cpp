#include "game.h"
#include <cmath>
#include <fstream>

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

    Image heartImage = LoadImage("serce.png");
    heartSprite = LoadTextureFromImage(heartImage);
    UnloadImage(heartImage);
    uiHeartScale = 3.5f;     // skala serc w pasku zdrowia gracza
    pickupHeartScale = 4.0f; // bazowa skala serc leżących na mapie

    Image itemImage = LoadImage("przedmiot.png");
    itemSprite = LoadTextureFromImage(itemImage);
    UnloadImage(itemImage);
    itemScale = 0.2f; 

    Image rockImage = LoadImage("kamien.png");
    rockSprite = LoadTextureFromImage(rockImage);
    UnloadImage(rockImage);
    rockScale = {4.0f, 2.5f}; // szerokosc / wysokosc 

    Image pBulletImage = LoadImage("pocisk.png");
    playerBulletSprite = LoadTextureFromImage(pBulletImage);
    UnloadImage(pBulletImage);
    Image eBulletImage = LoadImage("pocisk_wrog.png");
    enemyBulletSprite = LoadTextureFromImage(eBulletImage);
    UnloadImage(eBulletImage);
    bulletScale = 0.36f; 

    for(int i = 0; i < 5; i++) {
        hasEnemySprite[i] = false;
    }

    //wrog typ 3
    Image imageT3 = LoadImage("enemy.png"); 
    enemySprites[3] = LoadTextureFromImage(imageT3); 
    hasEnemySprite[3] = true; // Zaznaczamy, że typ 3 ma grafikę
    UnloadImage(imageT3);

    //wrog typ 2
    Image imageT2 = LoadImage("enemy2.png"); 
    enemySprites[2] = LoadTextureFromImage(imageT2); 
    hasEnemySprite[2] = true; 
    UnloadImage(imageT2);

    //wrog typ 1 (strzelający w 4 strony świata)
    Image imageT1 = LoadImage("enemy1.png"); 
    enemySprites[1] = LoadTextureFromImage(imageT1); 
    hasEnemySprite[1] = true; 
    UnloadImage(imageT1);

    //wrog typ 4 (boss)
    Image imageT4 = LoadImage("boss.png"); 
    enemySprites[4] = LoadTextureFromImage(imageT4); 
    hasEnemySprite[4] = true; 
    UnloadImage(imageT4);

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

    // Tymczasowo wyłączone ładowanie z pliku, aby uruchomić proceduralne generowanie
    // LoadRoomLayouts();

    isPaused = false;
    isGameOver = false;
    isVictory = false;
    clearedRoomsCount = 1;

    obstacles = {
        {roomX + 200, roomY + 150, 100 , 100},
        {roomX + 700, roomY + 300, 100 , 100}, 
        {roomX + 1000, roomY + 500, 100 , 100},
        {roomX + 1000, roomY + 600, 100 , 100}
    };

    enemies.push_back({{roomX + 700, roomY + 200}, 150.0f, 50.0f, 3, 2.0f, 5, 0, 0});
    enemies.push_back({{roomX + 400, roomY + 400}, 100.0f, 50.0f, 2, 2.0f, 5, 0, 0}); // Przeciwnik typu 2 
    enemies.push_back({{roomX + 1100, roomY + 300}, 120.0f, 50.0f, 1, 2.0f, 5, 0, 0}); // Przeciwnik typu 1 
    
    GenerateMap(); // Wywołujemy generację, aby połączyć drzwi do reszty mapy
    dungeonMap[{0, 0}].obstacles = obstacles;
    dungeonMap[{0, 0}].enemies = enemies;
}

void Game::LoadRoomLayouts() {
    std::ifstream file("layouts.txt");
    if (!file.is_open()) return;

    std::string line;
    std::vector<std::string> currentLayout;
    while (std::getline(file, line)) {
        // wymuszenie nowej mapy po -
        if (!line.empty() && line[0] == '-') {
            if (!currentLayout.empty()) roomLayouts.push_back(currentLayout);
            currentLayout.clear();
            continue;
        }
         
        // ignoruje puste linie tylko wtedy, gdy nie zaczęliśmy jeszcze czytać nowej mapy
        if (currentLayout.empty() && line.empty()) continue;

        // traktuje każdą inną linię jako część mapy 
        currentLayout.push_back(line);

        // kiedy miniemy 9 linii, zapisujemy gotową mapę
        if (currentLayout.size() == 9) {
            roomLayouts.push_back(currentLayout);
            currentLayout.clear();
        }
    }
    if (!currentLayout.empty()) {
        roomLayouts.push_back(currentLayout); 
    }
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
    UnloadTexture(heartSprite);
    UnloadTexture(itemSprite);
    UnloadTexture(rockSprite);
    UnloadTexture(playerBulletSprite);
    if (enemyBulletSprite.id != 0) UnloadTexture(enemyBulletSprite);
    UnloadTexture(enemyType3Sprite); 
    for(int i = 1; i <= 4; i++){
        if(hasEnemySprite[i]) UnloadTexture(enemySprites[i]);
    }
}