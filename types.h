#pragma once
#include "raylib.h"
#include <vector>

struct bullet{
    Vector2 position;
    Vector2 direction;
    float speed;
    bool isEnemy; 
    float attackRange = 10.0f;
};

struct enemy{
    Vector2 position;
    float speed;
    float size; 
    int type; 
    float shootTimer; 
    int hp;
    int burstBulletsLeft = 0;
    int burstInterval = 0.0f;
};

struct pickup {
    Vector2 position;
    int type; 
    bool active;
};

struct RoomData {
    int type = 0; 
    bool generated = false;
    bool cleared = false;
    bool heartSpawned = false;
    bool hasTop = false, hasBottom = false, hasLeft = false, hasRight = false;
    std::vector<enemy> enemies;
    std::vector<pickup> pickups;
    std::vector<Rectangle> obstacles;
};