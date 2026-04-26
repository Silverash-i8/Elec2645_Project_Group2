/**
 * @file Enemy.c
 * @brief Enemy Tank implementation for Tank 1990 game
 */

#include "Enemy.h"
#include "LCD.h"
#include "../Sprites/red_tank_sprites.h"
#include <stdlib.h>

void Enemy_Init(Enemy_t* enemy, uint16_t x, uint16_t y, uint8_t behavior) {
    Tank_Init(&enemy->tank, x, y, TANK_SPEED, 0);
    enemy->last_shot_time = 0;
    enemy->shoot_interval = ENEMY_SHOOT_INTERVAL;
    enemy->move_counter = 0;
    enemy->behavior = behavior;
    enemy->alive = 1;
}

Bullet_t* Enemy_Update(Enemy_t* enemy, uint16_t player_x, uint16_t player_y, uint16_t base_x, uint16_t base_y, uint8_t enemy_index, uint32_t current_time) {
    if (!enemy->alive) {
        return NULL;
    }

    // Simple AI: change direction every 40 frames based on target location
    enemy->move_counter++;
    if (enemy->move_counter > 20) {
        enemy->move_counter = 0;
        uint16_t target_x, target_y;
        if (enemy->behavior == 0) {
            // Chase Target : towards player
            target_x = player_x;
            target_y = player_y;
        } else if (enemy->behavior == 1) {
            target_x = base_x;
            target_y = base_y; 
        }
        // Calculate best direction towards target
        int16_t dx = target_x - enemy->tank.x;
        int16_t dy = target_y - enemy->tank.y;
        
        // Determine direction based on which distance is greater
        if (dx * dx + dy * dy == 0) {
            // Already at target, stay still
            enemy->tank.direction = 0;
        } else if (dx * dx > dy * dy) {
            // Move horizontally
            if (dx > 0) {
                enemy->tank.direction = 1; // Right
            } else {
                enemy->tank.direction = 3; // Left
            }
        } else {
            // Move vertically
            if (dy > 0) {
                enemy->tank.direction = 2; // Down
            } else {
                enemy->tank.direction = 0; // Up
            }
        }
    }

    // Create dummy joystick input for movement
    UserInput input;
    input.direction = CENTRE;
    
    switch (enemy->tank.direction) {
        case 0: input.direction = N; break;
        case 1: input.direction = W; break;
        case 2: input.direction = S; break;
        case 3: input.direction = E; break;
    }

    // Update enemy tanks position
    Tank_Update(&enemy->tank, input);

    // Simple firing AI - shoot occasionally
    Bullet_t* new_bullet = NULL;
    if ((current_time - enemy->last_shot_time) > enemy->shoot_interval) {
        enemy->last_shot_time = current_time;
        // Return signal that enemy wants to shoot (caller will manage bullet pool)
        new_bullet = (Bullet_t*)1; // Signal that bullet should be created
    }

    return new_bullet;
}

void Enemy_Draw(Enemy_t* enemy) {
    if (!enemy->alive) {
        return;
    }

    // Calculate position for top-left corner of sprite
    int16_t x_pos = enemy->tank.x - RED_TANK_WIDTH / 2;
    int16_t y_pos = enemy->tank.y - RED_TANK_HEIGHT / 2;
    
    // Select sprite based on enemy direction
    const uint8_t* sprite_data = NULL;
    
    switch (enemy->tank.direction) {
        case 0: sprite_data = (const uint8_t*)red_tank_up_data; break;       // UP
        case 1: sprite_data = (const uint8_t*)red_tank_right_data; break;   // RIGHT
        case 2: sprite_data = (const uint8_t*)red_tank_down_data; break;    // DOWN
        case 3: sprite_data = (const uint8_t*)red_tank_left_data; break;    // LEFT
        default: sprite_data = (const uint8_t*)red_tank_up_data; break;
    }
    
    // Draw the red tank sprite
    if (sprite_data != NULL) {
        LCD_Draw_Sprite(x_pos, y_pos, RED_TANK_HEIGHT, RED_TANK_WIDTH, sprite_data);
    }
}