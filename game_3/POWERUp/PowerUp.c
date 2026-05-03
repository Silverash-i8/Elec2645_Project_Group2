#include "PowerUp.h"
#include "PLAYER/Player.h"
#include "COLLISION/Collision.h"
#include "stm32l4xx_hal.h"
#include <stdlib.h>

static PowerUp powerup_pool[MAX_POWERUPS];

void PowerUp_Init(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerup_pool[i].active = false;
        powerup_pool[i].size = 6.0f; 
    }
}

void PowerUp_Spawn(float x, float y) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerup_pool[i].active) {
            powerup_pool[i].x = x;
            powerup_pool[i].y = y;
            powerup_pool[i].type = rand() % 3; // Randomly pick one of the 3 types
            powerup_pool[i].active = true;
            break; 
        }
    }
}

void PowerUp_Update(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerup_pool[i].active) {
            // Check if player picked it up
            if (Collision_CheckCircle(player.x, player.y, PLAYER_SIZE, 
                                      powerup_pool[i].x, powerup_pool[i].y, powerup_pool[i].size)) {
                
                // Apply the effect based on type
                switch (powerup_pool[i].type) {
                    case POWERUP_HEALTH:
                        player.health += 20;
                        if (player.health > PLAYER_MAX_HEALTH) player.health = PLAYER_MAX_HEALTH;
                        break;
                    case POWERUP_SPEED:
                        player.speed = 9.0f;                         // ~1.8x movement boost
                        player.speed_timer = HAL_GetTick() + 5000;   // lasts 5 seconds
                        break;
                    case POWERUP_RAPID_FIRE:
                        player.fire_rate = 150.0f;                         // 3x faster firing
                        player.rapid_fire_timer = HAL_GetTick() + 5000;    // lasts 5 seconds
                        break;
                }
                
                powerup_pool[i].active = false; // Item vanishes
            }
        }
    }
}

PowerUp* PowerUp_GetPool(void) {
    return powerup_pool;
}
