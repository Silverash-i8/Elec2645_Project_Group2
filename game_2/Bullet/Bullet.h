/**
 * @file Bullet.h
 * @brief Bullet system for Tank 1990 game
 * 
 * Manages projectile bullets fired by the player tank and enemy tanks.
 */

#ifndef BULLET_H
#define BULLET_H

#include <stdint.h>

#define MAX_BULLETS 10
#define BULLET_SIZE 4
#define BULLET_SPEED 9

typedef struct {
    uint16_t x;              // X position
    uint16_t y;              // Y position
    uint8_t direction;       // 0=up, 1=right, 2=down, 3=left
    uint8_t active;          // 1=active, 0=inactive
    uint8_t owner_id;        // 0 for player, 1 for enemies
} Bullet_t;

/**
 * @brief Initialize a bullet (set to inactive)
 * @param bullet Pointer to bullet structure
 */
void Bullet_Init(Bullet_t* bullet);

/**
 * @brief Fire a bullet from given position
 * @param bullet Pointer to bullet structure
 * @param x Starting X position
 * @param y Starting Y position
 * @param direction Direction to fire (0=up, 1=right, 2=down, 3=left)
 * @param owner_id Owner ID (0=player, 1=enemies)
 */
void Bullet_Fire(Bullet_t* bullet, uint16_t x, uint16_t y, uint8_t direction, uint8_t owner_id);

/**
 * @brief Update bullet position (move and check bounds)
 * @param bullet Pointer to bullet structure
 */
void Bullet_Update(Bullet_t* bullet);

/**
 * @brief Draw bullet on LCD
 * @param bullet Pointer to bullet structure
 */
void Bullet_Draw(Bullet_t* bullet);

#endif // BULLET_H