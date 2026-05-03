#include "Player.h"
#include "PWM.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"
#include <math.h>

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

void Player_Move(float joy_x, float joy_y) {
    if (player.is_dashing) {
        // 1. Slide the player using the fast dash speeds
        player.x += player.dash_vx;
        player.y += player.dash_vy;
        
        // 2. Check the clock
        uint32_t elapsed_time = HAL_GetTick() - player.dash_duration_timer;
        
        // Turn off the beep after 50ms
        if (elapsed_time > 50 && buzzer_is_running(&buzzer_cfg)) {
            buzzer_off(&buzzer_cfg);
        }

        // TURN OFF DASH MODE after 150ms! (This is what you were missing)
        if (elapsed_time > 150) {
            player.is_dashing = false; 
        }
        
   } else {
        // 3. NORMAL WALKING 
        player.x += joy_x * 5.0f; 
        player.y += joy_y * 5.0f; 

        // --- NEW: SPRITE DIRECTION LOGIC ---
        // Only change direction if the joystick is actually moving (deadzone check)
        if (fabs(joy_x) > 0.1f || fabs(joy_y) > 0.1f) {
            
            // Check if we are moving horizontally MORE than we are moving vertically
            if (fabs(joy_x) > fabs(joy_y)) {
                if (joy_x > 0) {
                    player.direction = 1; // RIGHT (You might need to tweak this number)
                } else {
                    player.direction = 3; // LEFT (You might need to tweak this number)
                }
            } 
            // Otherwise, we are moving vertically
            else {
                if (joy_y > 0) {
                    player.direction = 2; // DOWN (You might need to tweak this number)
                } else {
                    player.direction = 0; // UP (We know 0 is UP from your Init function)
                }
            }
        }
    }
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

void Player_StartDash(float joy_x, float joy_y) {
    if (!player.is_dashing) {
        player.is_dashing = true;
        player.dash_duration_timer = HAL_GetTick();
        
        // Set the speed of the dash
        player.dash_vx = joy_x * 15.0f;
        player.dash_vy = joy_y * 15.0f; 
        
        // Start the dash beep (1000Hz frequency, 50% volume)
        buzzer_tone(&buzzer_cfg, 1000, 50); 
    }
}
