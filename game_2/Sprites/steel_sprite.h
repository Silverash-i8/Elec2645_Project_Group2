#ifndef STEEL_SPRITE_H
#define STEEL_SPRITE_H

#include <stdint.h>

/* Steel sprite dimensions */
#define STEEL_FRAME_COUNT 1
#define STEEL_FRAME_WIDTH 10
#define STEEL_FRAME_HEIGHT 10

/* Steel sprite data */
const uint8_t steel_data[10][10] = {
    { 0, 13, 13, 13, 13, 13, 13, 13, 13, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 13, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 13, 13, 13 },
    { 0, 1, 1, 1, 1, 1, 1, 13, 13, 13 },
    { 0, 1, 1, 1, 1, 1, 1, 13, 13, 13 },
    { 0, 1, 1, 1, 1, 1, 1, 13, 13, 13 },
    { 0, 1, 1, 1, 1, 1, 1, 13, 13, 13 },
    { 0, 1, 13, 13, 13, 13, 13, 13, 13, 13 },
    { 0, 13, 13, 13, 13, 13, 13, 13, 13, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

#endif
