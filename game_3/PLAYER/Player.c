#include "Player.h"
#include "PWM.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"

extern PWM_cfg_t pwm_cfg;
extern Buzzer_cfg_t buzzer_cfg;

// Global player instance
Player player;

void Player_Init(void) {
    player.x = 0; // Start at world origin
    player.y = 0;
    player.health = PLAYER_MAX_HEALTH;
    player.score = 0;
    player.wave = 0;
    player.last_shot = 0;
    player.wave_timer = HAL_GetTick();
    player.enemy_spawn_timer = HAL_GetTick();
    player.last_dash = 0;
    player.direction = 0; // Start facing up
}

void Player_Move(float dx, float dy) {
    player.x += dx * PLAYER_SPEED;
    player.y += dy * PLAYER_SPEED;

    // Update direction based on movement
    if (dx > 0.1f) player.direction = 1; // right
    else if (dx < -0.1f) player.direction = 3; // left
    else if (dy > 0.1f) player.direction = 2; // down
    else if (dy < -0.1f) player.direction = 0; // up
   
}

void Player_TakeDamage(float damage) {
    player.health -= damage;
    if (player.health < 0) player.health = 0;
}

void Player_AddScore(int points) {
    player.score += points;
}

bool Player_IsAlive(void) {
    return player.health > 0;
}

void Player_UpdateTimers(uint32_t current_time) {
    (void)current_time;
}

void Player_Dash(void) {
    uint32_t current_time = HAL_GetTick();
    
    // 1. Check Cooldown
    if (current_time - player.last_dash < DASH_COOLDOWN) {
        return; // Too soon to dash again
    }

    switch(player.direction) {
        case 0: player.y -= DASH_DISTANCE; break; // Up
        case 1: player.x += DASH_DISTANCE; break; // Right
        case 2: player.y += DASH_DISTANCE; break; // Down
        case 3: player.x -= DASH_DISTANCE; break; // Left
    }

    // 2. Reset the timer
    player.last_dash = current_time;
    
    // 3. Play dash sound effect

}
