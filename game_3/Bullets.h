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
    bool active;
} Bullet;

// Sets all bullets to inactive at game start
void Bullet_Init(void);

// Logic to move bullets and deactivate them if they leave the screen
void Bullet_Update(void);

// This is the auto-fire function that spawns bullets at regular intervals
void Bullet_Spawn(float x, float y, float vx, float vy);

#endif