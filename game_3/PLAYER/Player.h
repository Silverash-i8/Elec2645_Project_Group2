#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <stdbool.h>

// Game constants
#define PLAYER_SIZE 14
#define PLAYER_SPEED 3.0f
#define PLAYER_MAX_HEALTH 100.0f
#define BULLET_SPEED 3.5f
#define BULLET_DAMAGE 1.0f
#define DASH_DISTANCE 40.0f
#define DASH_COOLDOWN 1000

// Player state structure
typedef struct {
    float x, y;        
    float health;
    int score;
    int wave;
    uint32_t last_shot;
    uint32_t wave_timer;
    uint32_t enemy_spawn_timer;
    uint32_t last_dash;
    bool is_dashing;
    uint32_t dash_duration_timer; 
    float dash_vx; // Dash velocity X
    float dash_vy; // Dash velocity Y
    int direction;
    float speed;                 // Movement speed (boosted by POWERUP_SPEED)
    float fire_rate;             // Firing interval in ms (reduced by POWERUP_RAPID_FIRE)
    uint32_t speed_timer;        // Tick when speed boost expires (0 = inactive)
    uint32_t rapid_fire_timer;   // Tick when rapid-fire boost expires (0 = inactive)
} Player;

// Global player instance
extern Player player;

// Player functions
void Player_Init(void);
void Player_Move(float dx, float dy);
void Player_TakeDamage(float damage);
void Player_AddScore(int points);
bool Player_IsAlive(void);
void Player_UpdateTimers(uint32_t current_time);
void Player_StartDash(float joy_x, float joy_y);
#endif // PLAYER_H