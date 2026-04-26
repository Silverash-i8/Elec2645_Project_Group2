/**
 * @file Enemy.h
 * @brief Enemy Tank system for Tank 1990 game
 * 
 * Manages AI-controlled enemy tanks with movement and firing behavior.
 */

#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>
#include "../Tank/Tank.h"
#include "../Bullet/Bullet.h"
#define MAX_ENEMIES 6              // Maximum number of enemies on screen
#define ENEMY_SHOOT_INTERVAL 1000  // ms between shots

typedef struct {
    Tank_t tank;                    // Tank object
    uint32_t last_shot_time;        // Time of last shot
    uint16_t shoot_interval;        // Interval between shots (ms)
    uint8_t move_counter;           // Counter for AI movement decisions
    uint8_t behavior;               // Behavior type (0=towards player, 1=towards base)
    uint8_t alive;                  // 1 if alive, 0 if destroyed
    uint32_t chase_start_time;      // Time when chase behavior started
    uint8_t chase_direction_index;  // Current direction in chase cycle (0-3)
} Enemy_t;

/**
 * @brief Initialize an enemy tank
 * @param enemy Pointer to enemy structure
 * @param x Starting X position
 * @param y Starting Y position
 * @param behavior Behavior type (0=random, 1=towards player)
 */
void Enemy_Init(Enemy_t* enemy, uint16_t x, uint16_t y, uint8_t behavior);

/**
 * @brief Update enemy position and AI logic
 * @param enemy Pointer to enemy structure
 * @param player_x Player tank X position
 * @param player_y Player tank Y position
 * @param base_x Base X position
 * @param base_y Base Y position
 * @param enemy_index Index of this enemy (used to determine targeting)
 * @param current_time Current system time (ms)
 * @return Pointer to a bullet if fired this frame, NULL otherwise
 */
Bullet_t* Enemy_Update(Enemy_t* enemy, uint16_t player_x, uint16_t player_y, uint16_t base_x, uint16_t base_y, uint8_t enemy_index, uint32_t current_time);

/**
 * @brief Draw enemy tank
 * @param enemy Pointer to enemy structure
 */
void Enemy_Draw(Enemy_t* enemy);

#endif