/**
 * @file TankEngine.h
 * @brief Main game engine for Tank 1990 game
 * Coordinates tank and map updates, handles collisions, and manages game state.
 */

#ifndef TANKENGINE_H
#define TANKENGINE_H

#include <stdint.h>
#include "../Tank/Tank.h"
#include "../Map/Map.h"
#include "../Bullet/Bullet.h"
#include "../Enemy/Enemy.h"
#include "Joystick.h"
/**
 * @struct TankEngine_t
 * @brief Main game engine object managing the game state
 */
typedef struct {
    Tank_t player;              // Player Tank object
    Enemy_t enemies[MAX_ENEMIES];  // Array of enemy tanks
    uint8_t num_enemies;        // Number of active enemies
    
    Bullet_t bullets[MAX_BULLETS];  // Array of all bullets
    uint8_t lives;              // Remaining lives (game over when 0)
    uint16_t score;             // Player's score
    uint32_t last_shot_time;    // Time of last player shot
    uint8_t base_health;        // Base health (destroyed at 0)
    uint8_t game_over;          // 1 if game over, 0 if playing
    uint8_t player_won;         // 1 if all enemies destroyed
    uint32_t freeze_until_tick; // If > current tick, game is frozen (e.g. after player hit)
    uint8_t freeze_reason;      // 0=none, 1=player hit (respawn), 2=base destroyed (no respawn)
    uint16_t respawn_x;         // Tank respawn X coordinate
    uint16_t respawn_y;         // Tank respawn Y coordinate
} TankEngine_t;

/**
 * @brief Initialize the Tank game engine
 * Sets up the map grid, spawns the tank at the start position,
 * and resets lives and score.
 * @param engine Pointer to game engine
 * @param start_x Initial X pixel coordinate for the tank
 * @param start_y Initial Y pixel coordinate for the tank
 * @param speed Movement speed of the tank
 * @param direction Initial facing direction (0=Up, 1=Right, 2=Down, 3=Left)
 */
void TankEngine_Init(TankEngine_t* engine, uint16_t start_x, uint16_t start_y, int16_t speed, uint8_t direction);

/**
 * @brief Update game state (input + physics + collisions)
 * This is called every frame:
 * 1. Updates tank based on joystick input
 * 2. Updates enemies and their AI
 * 3. Updates bullets
 * 4. Checks all collisions
 * 5. Manages audio and game states
 * @param engine Pointer to game engine
 * @param input Joystick input for this frame
 */
void TankEngine_Update(TankEngine_t* engine, UserInput input);

/**
 * @brief Draw all game objects
 * Draws the map and all game objects to the LCD buffer.
 * Note: does NOT call LCD_Fill_Buffer() or LCD_Refresh() - 
 * those are main.c's responsibility.
 * @param engine Pointer to game engine
 */
void TankEngine_Draw(TankEngine_t* engine);

/**
 * @brief Fire a bullet from the player tank
 * @param engine Pointer to game engine
 */
void TankEngine_PlayerFire(TankEngine_t* engine);

/**
 * @brief Get current lives remaining
 * @param engine Pointer to game engine
 * @return Number of lives
 */
uint8_t TankEngine_GetLives(TankEngine_t* engine);

/**
 * @brief Get current score
 * @param engine Pointer to game engine
 * @return Current score
 */
uint16_t TankEngine_GetScore(TankEngine_t* engine);

/**
 * @brief Get current base health
 * @param engine Pointer to game engine
 * @return Base health (0-100)
 */
uint16_t TankEngine_GetBaseHealth(TankEngine_t* engine);

/**
 * @brief Check if game is over
 * @param engine Pointer to game engine
 * @return 1 if game over, 0 if still playing
 */
uint8_t TankEngine_IsGameOver(TankEngine_t* engine);

/**
 * @brief Check if player won (all enemies destroyed)
 * @param engine Pointer to game engine
 * @return 1 if won, 0 otherwise
 */
uint8_t TankEngine_PlayerWon(TankEngine_t* engine);

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
                                 int16_t rectX,   int16_t rectY,   uint16_t rectW, uint16_t rectH);
/**
 * @brief Check if bullet hits a tank
 */
uint8_t BulletHitsTank(Bullet_t* bullet, Tank_t* tank);
/**
 * @brief Find first available bullet slot
 */
int16_t FindAvailableBulletSlot(TankEngine_t* engine);

#endif