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
    // If no movement, keep previous direction
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
    // This function can be expanded if needed for timer management
    (void)current_time; // Suppress unused parameter warning
}