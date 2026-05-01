#ifndef BULLET_H
#define BULLET_H

#include <stdint.h>
#include <stdbool.h>
#include "LCD.h"

// Sets a maximum no of bullets to conserve space
#define MAX_BULLETS 50

typedef struct {
    float x, y;
    float vx, vy;
    float damage;
    bool active;
} Bullet;

// Sets all bullets to inactive at game start
void Bullet3_Init(void);

// Logic to move bullets and deactivate them if they leave the screen
void Bullet3_Update(void);

// This is the auto-fire function that spawns bullets at regular intervals
void Bullet3_Spawn(float x, float y, float vx, float vy, float damage);

// Get pointer to bullet pool for collision detection
Bullet* Bullet3_GetPool(void);

#endif