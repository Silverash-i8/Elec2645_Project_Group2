#include "PowerUp.h"
#include "PLAYER/Player.h"
#include "COLLISION/Collision.h"
#include <stdlib.h> // For rand()

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
                        // You'll need a speed variable in your Player struct for this!
                        break;
                    case POWERUP_RAPID_FIRE:
                        // You'll need a fire-rate variable in your Player struct for this!
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
