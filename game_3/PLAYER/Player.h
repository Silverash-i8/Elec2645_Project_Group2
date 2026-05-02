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
    int direction;     
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
void Player_Dash(void);

#endif // PLAYER_H