#ifndef BRICK_SPRITE_H
#define BRICK_SPRITE_H

#include <stdint.h>

/* Brick sprite dimensions */
#define BRICK_FRAME_WIDTH 10
#define BRICK_FRAME_HEIGHT 10

/* Sprite data for "brick" */
const uint8_t brick_data[10][10] = {
    { 12, 12, 12, 12, 12, 13, 12, 12, 12, 12 },
    { 2, 2, 2, 2, 2, 13, 12, 12, 2, 2 },
    { 2, 2, 2, 2, 2, 13, 12, 12, 2, 2 },
    { 2, 2, 2, 2, 2, 13, 12, 12, 2, 2 },
    { 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
    { 13, 12, 12, 12, 12, 12, 12, 12, 12, 12 },
    { 13, 12, 12, 2, 2, 2, 2, 2, 2, 2 },
    { 13, 12, 12, 2, 2, 2, 2, 2, 2, 2 },
    { 13, 12, 12, 2, 2, 2, 2, 2, 2, 2 },
    { 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 }
};

#endif
