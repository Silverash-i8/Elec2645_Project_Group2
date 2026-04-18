#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <stdint.h>

typedef struct {
    uint16_t x, y;
    uint8_t current_frame;
    uint32_t last_tick;
    uint8_t active;
} Explosion_t;

// Allow for multiple explosions at once 
#define MAX_EXPLOSIONS 3

/**
 * @brief Draw all active explosions
 */
void Explosions_Draw();

/**
 * @brief Update all active explosions
 */
void Explosions_Update();

/**
 * @brief Start a new explosion at the specified coordinates
 * @param x X-coordinate of the explosion
 * @param y Y-coordinate of the explosion
 */
void Explosions_Start(uint16_t x, uint16_t y);

/**
 * @brief Clear all active explosions
 */
void Explosions_Clear();
#endif