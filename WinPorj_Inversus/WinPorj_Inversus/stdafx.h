#pragma once

#define _USE_MATH_DEFINES

#include <Windows.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>

// Map
#define ROW 15
#define E_COL 16
#define N_COL 20
#define MAP_SIZE 30
#define MAP_LOCATION 50
#define EASY 0
#define NORMAL 1
#define HARD 2
#define WHITE 0
#define BLACK 1
#define RED 2
#define WALL 3

// Object
#define LEFT 100
#define RIGHT 101
#define UP 102
#define DOWN 103

// Player
#define MAX_BULLET 6
#define RESPAWN_TIME 500
#define SPEED 4
#define PLAYER_SIZE 13
#define BULLET_SIZE 2
#define BULLET_RELOAD_TIME 3000
#define BULLET_RENDER_TIME 40

// Bullet
#define BULLET_SPEED 5
#define MAX_BULLET_DRAW 12
#define BULLET_MOVE_TIME 10

// Enemy
#define ENEMY_SPAWN_TIME 2000
#define ENEMY_SPEED 2
#define MAX_ENEMY 50
#define ENEMY_MOVE_TIME 50

// TIME
#define BULLET_RELOAD 999
#define BULLET_RENDER 1000
#define BULLET_MOVE 1001

#define ENEMY_MOVE 1002
#define ENEMY_SPAWN 1004

#define PLAYER_RESPAWN 1003

typedef struct BulletInfo
{
    POINT _position;
    int _direction;
} BulletInfo;