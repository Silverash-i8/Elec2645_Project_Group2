/**
 * @file TankEngine.c
 * @brief Main game engine implementation for Tank 1990
 */

#include "TankEngine.h"
#include "Buzzer.h"
#include "LCD.h"
#include "Map.h"
#include <stdio.h>
#include "Explosion.h"
#include "stm32l4xx_hal.h"

#define BUZZER_SHOOT_FREQ_HZ 1200
#define BUZZER_HIT_FREQ_HZ 800
#define BUZZER_VOLUME 50
#define BUZZER_BEEP_MS 40
#define PLAYER_SHOOT_COOLDOWN_MS 1000

extern Buzzer_cfg_t buzzer_cfg;
static uint32_t buzzer_stop_tick = 0;

static void TankEngine_Beep(uint32_t freq_hz) {
    buzzer_tone(&buzzer_cfg, freq_hz, BUZZER_VOLUME);
    buzzer_stop_tick = HAL_GetTick() + BUZZER_BEEP_MS;
}

static void TankEngine_UpdateBuzzer(void) {
    if (buzzer_stop_tick != 0 && (int32_t)(HAL_GetTick() - buzzer_stop_tick) >= 0) {
        buzzer_off(&buzzer_cfg);
        buzzer_stop_tick = 0;
    }
}

/**
 * @brief Checks for collision between a circle and an axis-aligned rectangle.
 * * @param circleX   Center X of the circle
 * @param circleY   Center Y of the circle
 * @param radius    Radius of the circle
 * @param rectX     Center X of the rectangle
 * @param rectY     Center Y of the rectangle
 * @param rectW     Total width of the rectangle
 * @param rectH     Total height of the rectangle
 * @return uint8_t  1 if colliding, 0 if not
 */
uint8_t CheckCircleRectCollision(int16_t circleX, int16_t circleY, uint16_t radius,
                                 int16_t rectX,   int16_t rectY,   uint16_t rectW, uint16_t rectH) {
    
    // 1. Calculate the rectangle's edges
    int16_t halfW = rectW / 2;
    int16_t halfH = rectH / 2;
    
    int16_t rectMinX = rectX - halfW;
    int16_t rectMaxX = rectX + halfW;
    int16_t rectMinY = rectY - halfH;
    int16_t rectMaxY = rectY + halfH;

    // 2. Find the closest point on the rectangle to the circle center
    int16_t closestX = circleX;
    int16_t closestY = circleY;

    if (circleX < rectMinX){
         closestX = rectMinX; 
    }
    else if (circleX > rectMaxX){
        closestX = rectMaxX;
    }

    if (circleY < rectMinY){
        closestY = rectMinY;
    }
    else if (circleY > rectMaxY){
        closestY = rectMaxY;
    }

    // 3. Calculate squared distance between circle center and this closest point
    int32_t dx = circleX - closestX;
    int32_t dy = circleY - closestY;
    
    // 4. Return true (1) if distance is less than radius squared
    return (dx * dx + dy * dy) < (int32_t)(radius * radius);
}

/**
 * @brief Check if bullet hits a tank
 */
static uint8_t BulletHitsTank(Bullet_t* bullet, Tank_t* tank) {
    if (!bullet->active || !tank->alive) {
        return 0;
    }
    return  CheckCircleRectCollision(
        bullet->x, bullet->y, BULLET_SIZE,
        tank->x,   tank->y,   TANK_SIZE, TANK_SIZE
    );
}

/**
 * @brief Find first available bullet slot
 */
static int16_t FindAvailableBulletSlot(TankEngine_t* engine) {
    for (int16_t i = 0; i < MAX_BULLETS; i++) {
        if (!engine->bullets[i].active) {
            return i;
        }
    }
    return -1; // No available slot
}

// ==========================================
// PUBLIC API
// ==========================================

void TankEngine_Init(TankEngine_t* engine, uint16_t start_x, uint16_t start_y, int16_t speed, uint8_t direction) {

    // Initialize player tank
    Tank_Init(&engine->player, start_x, start_y, speed, direction);
    
    // Initialize all bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        Bullet_Init(&engine->bullets[i]);
    }
    
    // Define spawn positions for enemies (supports up to MAX_ENEMIES)
    // Spawned in safe corners away from bricks
    const struct {
        uint16_t x;
        uint16_t y;
        uint8_t behavior;
    } spawn_positions[MAX_ENEMIES] = {
        // Top-left corner
        {60, MAP_OFFSET_Y + 40, 0},
        // Top-right corner
        {MAP_WIDTH * TILE_SIZE - 60, MAP_OFFSET_Y + 40, 1},
        // Bottom-left corner
        {70, MAP_OFFSET_Y + MAP_HEIGHT * TILE_SIZE - 60, 0},
        // Bottom-right corner
        {MAP_WIDTH * TILE_SIZE - 30, MAP_OFFSET_Y + MAP_HEIGHT * TILE_SIZE - 30, 1},
        // Middle-top (for medium/hard modes)
        {MAP_WIDTH * TILE_SIZE / 2 + 40, MAP_OFFSET_Y + 60, 0},
        // Middle-bottom (for hard mode only)
        {MAP_WIDTH * TILE_SIZE / 2, MAP_OFFSET_Y + MAP_HEIGHT * TILE_SIZE - 70, 1}
    };
    
    // Initialize only the required number of enemies
    for (uint8_t i = 0; i < engine->num_enemies; i++) {
        Enemy_Init(&engine->enemies[i], 
                   spawn_positions[i].x, 
                   spawn_positions[i].y, 
                   spawn_positions[i].behavior);
    }

    engine->lives = 3;
    engine->score = 0;
    engine->last_shot_time = 0;
    engine->base_health = 2;
    engine->game_over = 0;
    engine->player_won = 0;
    engine->freeze_until_tick = 0; // Initialize freeze timer
    engine->freeze_reason = 0;     // Initialize freeze reason (0=none)
    engine->respawn_x = 0;         // Initialize respawn position
    engine->respawn_y = 0;         // Initialize respawn position
}

void TankEngine_Update(TankEngine_t* engine, UserInput input) {
    uint32_t current_time = HAL_GetTick();

    // 1. ALWAYS update effects, even if game is frozen or over!
    Explosions_Update();
    TankEngine_UpdateBuzzer();

    // 2. Check for Freeze state (MUST be before game_over check so freeze works even after base destroyed)
    if (engine->freeze_until_tick > 0) {
        if (current_time < engine->freeze_until_tick) {
            // We are still frozen. Return early so nothing moves.
            return; 
        } else {
            // Freeze time is up! Reset the timer and resume game logic.
            engine->freeze_until_tick = 0;
            
            // Handle respawn only if freeze reason was player hit (not base destroyed)
            if (engine->freeze_reason == 1) {
                Tank_Init(&engine->player, engine->respawn_x, engine->respawn_y, TANK_SPEED, 0);
            } else if (engine->freeze_reason == 2) {
                // Freeze for base destruction is over - NOW set game over
                engine->game_over = 1;
            }
            engine->freeze_reason = 0; // Clear freeze reason
        }
    }

    // 3. Check for Game Over state
    if (engine->game_over) {
        return; 
    }

    // Update player tank
    Tank_Update(&engine->player, input);

    // Update all enemies
    // Base location (bottom center of map)
    uint16_t base_x = (MAP_WIDTH * TILE_SIZE / 2);
    uint16_t base_y = MAP_OFFSET_Y + (MAP_HEIGHT * TILE_SIZE);
    
    for (uint8_t i = 0; i < engine->num_enemies; i++) {
        if (engine->enemies[i].alive) {
            Bullet_t* enemy_bullet = Enemy_Update(&engine->enemies[i], 
                                                   engine->player.x, 
                                                   engine->player.y,
                                                   base_x,
                                                   base_y,
                                                   i,
                                                   current_time);
            
            // Enemy wants to shoot
            if (enemy_bullet != NULL) {
                int16_t slot = FindAvailableBulletSlot(engine);
                if (slot >= 0) {
                    Bullet_Fire(&engine->bullets[slot], 
                               engine->enemies[i].tank.x, 
                               engine->enemies[i].tank.y, 
                               engine->enemies[i].tank.direction, 
                               i + 1); // Owner ID (0=player, 1+=enemies)
                    
                }
            }
        }
    }

    // Update all bullets
    for (uint8_t i = 0; i < MAX_BULLETS; i++) {
        if (engine->bullets[i].active) {
            Bullet_Update(&engine->bullets[i]);
        }
    }

    // ===== COLLISION DETECTION =====

    // Check player bullets hitting enemies
    for (uint8_t b = 0; b < MAX_BULLETS; b++) {
        if (!engine->bullets[b].active || engine->bullets[b].owner_id != 0) {
            continue; // Only check player bullets
        }

        for (uint8_t e = 0; e < engine->num_enemies; e++) {
            if (engine->enemies[e].alive && BulletHitsTank(&engine->bullets[b], &engine->enemies[e].tank)) {
                // Hit! Destroy both
                engine->bullets[b].active = 0;
                engine->enemies[e].alive = 0;
                engine->enemies[e].tank.alive = 0;
                engine->score += 100;
                TankEngine_Beep(BUZZER_HIT_FREQ_HZ);
                Explosions_Start(engine->enemies[e].tank.x, engine->enemies[e].tank.y);
            }
        }
    }

    // Check enemy bullets hitting player
    for (uint8_t b = 0; b < MAX_BULLETS; b++) {
        if (!engine->bullets[b].active || engine->bullets[b].owner_id == 0) {
            continue; // Only check enemy bullets
        }

        if (BulletHitsTank(&engine->bullets[b], &engine->player)) {
            // Player hit! Lose a life
            engine->bullets[b].active = 0;
            engine->lives--;
            engine->player.alive = 0;
            TankEngine_Beep(BUZZER_HIT_FREQ_HZ);
            Explosions_Start(engine->player.x, engine->player.y);
            
            // Trigger 2-second freeze
            engine->freeze_until_tick = HAL_GetTick() + 2000;
            engine->freeze_reason = 1;  // Freeze reason: player hit (will respawn)

            if (engine->lives == 0) {
                engine->game_over = 1;
            } else {
                // Store respawn position (actual respawn happens after freeze ends)
                engine->respawn_x = 22;
                engine->respawn_y = 220;
            }
        }
    }

    // Check bullets hitting base
    for (uint8_t b = 0; b < MAX_BULLETS; b++) {
        if (!engine->bullets[b].active) {
            continue;
        }
        // Check if bullet hits the base area (we can define it as the bottom center of the map)
        if (engine->bullets[b].y > MAP_OFFSET_Y + (MAP_HEIGHT * TILE_SIZE) - 30 &&
            engine->bullets[b].x > (MAP_WIDTH * TILE_SIZE / 2) - 20 &&
            engine->bullets[b].x < (MAP_WIDTH * TILE_SIZE / 2) + 20) {
            
            // Base hit!
            engine->base_health--;
            engine->bullets[b].active = 0;
            
            if (engine->base_health == 1) {
                TankEngine_Beep(400);  // Warning tone on first hit (lower frequency)
            } else if (engine->base_health == 0) {
                // Base destroyed - game over
                TankEngine_Beep(1200);  // Destruction sound on second hit
                Explosions_Start(base_x, base_y);
                
                // Trigger 2-second freeze so player can see base explosion
                engine->freeze_until_tick = HAL_GetTick() + 2000;
                engine->freeze_reason = 2;  // Freeze reason: base destroyed (no respawn)
                // DO NOT set game_over = 1 here! It will be set after freeze expires
            }
        }
    }

    // Check if all enemies defeated
    uint8_t all_dead = 1;
    for (uint8_t i = 0; i < engine->num_enemies; i++) {
        if (engine->enemies[i].alive) {
            all_dead = 0;
            break;
        }
    }
    if (all_dead) {
        engine->player_won = 1;
        engine->game_over = 1;
    }
}

void TankEngine_Draw(TankEngine_t* engine) {
    // Draw map (pass base_health so base sprite only draws when alive)
    Map_Draw(engine->base_health);

    // Draw player tank (only if alive)
    if (engine->player.alive) {
        Tank_Draw(&engine->player);
    }

    // Draw enemies
    for (uint8_t i = 0; i < engine->num_enemies; i++) {
        Enemy_Draw(&engine->enemies[i]);
    }

    // Draw all bullets
    for (uint8_t i = 0; i < MAX_BULLETS; i++) {
        Bullet_Draw(&engine->bullets[i]);
    }
    
    // Draw all active explosions
    Explosions_Draw();
}

void TankEngine_PlayerFire(TankEngine_t* engine) {
    // Don't allow shooting while frozen or dead
    if (engine->freeze_until_tick > 0 || !engine->player.alive) {
        return;
    }

    uint32_t current_time = HAL_GetTick();
    
    if ((current_time - engine->last_shot_time) < PLAYER_SHOOT_COOLDOWN_MS) {
        return; // Can't shoot yet
    }

    int16_t slot = FindAvailableBulletSlot(engine);
    if (slot >= 0) {
        Bullet_Fire(&engine->bullets[slot], 
                   engine->player.x, 
                   engine->player.y, 
                   engine->player.direction, 
                   0); // Owner ID 0 = player
        engine->last_shot_time = current_time;
        TankEngine_Beep(BUZZER_SHOOT_FREQ_HZ);
    }
}

uint8_t TankEngine_GetLives(TankEngine_t* engine) {
    return engine->lives;
}

uint16_t TankEngine_GetScore(TankEngine_t* engine) {
    return engine->score;
}

uint16_t TankEngine_GetBaseHealth(TankEngine_t* engine) {
    return engine->base_health;
}

uint8_t TankEngine_IsGameOver(TankEngine_t* engine) {
    return engine->game_over;
}

uint8_t TankEngine_PlayerWon(TankEngine_t* engine) {
    return engine->player_won;
}