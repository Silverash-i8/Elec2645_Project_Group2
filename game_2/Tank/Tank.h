/**
 * @file Tank.h
 * @brief Main header file for Tank 1990 game
 * 
 * Controls Tank-related functionality.
 */

#ifndef TANK_H
#define TANK_H

#include <stdint.h>
#include "Joystick.h"
#include "Utils.h"

#define TANK_SIZE 16
#define TANK_SPEED 5

typedef struct{
    uint16_t x; // Tank X position (center)
    uint16_t y; // Tank Y position (center)
    int16_t speed; // Movement speed in pixels/frame
    uint8_t direction; // 0=up, 1=right, 2=down, 3=left
    uint8_t alive; // 1 if tank is alive, 0 if destroyed
    uint8_t animation_frame; // 0=IDLE, 1=MOVE for track animation
} Tank_t;

void Tank_Init(Tank_t* tank, uint16_t x, uint16_t y, int16_t speed, uint8_t direction);


void Tank_Update(Tank_t* tank, UserInput input);
void Tank_Draw(Tank_t* tank);

#endif