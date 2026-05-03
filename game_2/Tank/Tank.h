/**
 * @file Tank.h
 * @brief Player's Tank-related functionality for the Tank 1990 game
 */

#ifndef TANK_H
#define TANK_H

#include <stdint.h>
#include "Joystick.h"
#define TANK_SIZE 16
#define TANK_SPEED 5  // Player Speed in pixels per frame

typedef struct{
    uint16_t x; // Tank X position (center)
    uint16_t y; // Tank Y position (center)
    int16_t speed; // Movement speed in pixels/frame
    uint8_t direction; // 0=up, 1=right, 2=down, 3=left
    uint8_t alive; // 1 if tank is alive, 0 if destroyed
} Tank_t;

/**
 * @brief Initialize a tank with the specified parameters
 * @param tank Pointer to the tank structure
 * @param x Initial X position
 * @param y Initial Y position
 * @param speed Movement speed in pixels/frame
 * @param direction Initial direction (0=up, 1=right, 2=down, 3=left)
 */
void Tank_Init(Tank_t* tank, uint16_t x, uint16_t y, int16_t speed, uint8_t direction);

/**
 * @brief Update the tank's position and state based on user input
 * @param tank Pointer to the tank structure
 * @param input User input structure containing joystick direction and button states
 */
void Tank_Update(Tank_t* tank, UserInput input);
/**
 * @brief Draw the tank on the LCD based on its current state
 * @param tank Pointer to the tank structure
 */
void Tank_Draw(Tank_t* tank);

#endif