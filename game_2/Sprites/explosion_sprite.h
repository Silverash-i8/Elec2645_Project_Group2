#ifndef EXPLOSION_SPRITE_H
#define EXPLOSION_SPRITE_H

#include <stdint.h>

/* Small explosion sprite dimensions */
#define EXPLOSION_FRAME1_WIDTH 10
#define EXPLOSION_FRAME1_HEIGHT 10

/* Medium explosion sprite dimensions */
#define EXPLOSION_FRAME2_WIDTH 13
#define EXPLOSION_FRAME2_HEIGHT 13

/* Large explosion sprite dimensions */
#define EXPLOSION_FRAME3_WIDTH 16
#define EXPLOSION_FRAME3_HEIGHT 16


/* Small Explosion */
const uint8_t explosion_frame1[10][10] = {
    { 0, 0, 0, 13, 13, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 13, 13, 0, 13, 13, 1, 0 },
    { 8, 7, 7, 8, 7, 8, 13, 1, 8, 0 },
    { 0, 8, 8, 1, 7, 8, 7, 8, 13, 13 },
    { 13, 13, 7, 7, 7, 8, 8, 8, 7, 13 },
    { 13, 13, 7, 7, 8, 2, 8, 8, 12, 0 },
    { 0, 0, 7, 7, 8, 8, 8, 7, 8, 0 },
    { 0, 13, 8, 13, 7, 7, 1, 7, 7, 8 },
    { 1, 8, 0, 8, 13, 8, 7, 13, 0, 1 },
    { 0, 0, 13, 13, 0, 0, 8, 0, 0, 0 }
};

/* Medium Explosion */
const uint8_t explosion_frame2[13][13] = {
    { 0, 0, 0, 0, 0, 8, 0, 0, 13, 13, 0, 0, 0 },
    { 0, 7, 13, 12, 0, 7, 8, 0, 7, 12, 0, 13, 7 },
    { 0, 8, 7, 13, 12, 8, 1, 13, 7, 8, 7, 7, 8 },
    { 0, 0, 8, 7, 8, 7, 1, 1, 8, 8, 7, 8, 12 },
    { 0, 12, 0, 8, 7, 8, 13, 8, 7, 7, 8, 0, 0 },
    { 0, 0, 13, 1, 1, 1, 2, 2, 2, 12, 7, 1, 1 },
    { 1, 1, 13, 7, 7, 0, 2, 2, 13, 1, 8, 8, 0 },
    { 0, 0, 8, 7, 1, 8, 2, 2, 8, 7, 8, 0, 8 },
    { 0, 0, 0, 13, 7, 7, 8, 13, 8, 7, 7, 13, 0 },
    { 0, 0, 12, 8, 7, 7, 7, 7, 1, 8, 8, 7, 13 },
    { 0, 0, 13, 7, 8, 1, 8, 12, 7, 13, 0, 0, 8 },
    { 0, 13, 7, 8, 0, 1, 0, 8, 12, 8, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

/* Large Explosion */
const uint8_t explosion_frame3[16][16] = {
    { 0, 0, 0, 0, 8, 0, 8, 0, 0, 0, 0, 8, 0, 0, 8, 0 },
    { 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 8, 0, 0, 1, 8, 0 },
    { 0, 8, 8, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 8, 0, 0 },
    { 0, 0, 8, 8, 1, 8, 8, 1, 8, 8, 0, 1, 1, 8, 0, 0 },
    { 0, 0, 0, 8, 2, 1, 1, 1, 1, 8, 1, 1, 8, 8, 0, 8 },
    { 0, 1, 0, 8, 1, 1, 2, 1, 0, 1, 1, 2, 8, 0, 0, 0 },
    { 0, 0, 8, 1, 1, 2, 8, 2, 2, 0, 2, 8, 1, 8, 0, 0 },
    { 1, 1, 1, 1, 0, 1, 2, 0, 0, 2, 1, 1, 1, 1, 1, 1 },
    { 0, 8, 8, 8, 1, 1, 8, 2, 0, 2, 1, 8, 8, 8, 0, 0 },
    { 0, 0, 0, 8, 8, 8, 0, 1, 2, 0, 8, 8, 1, 1, 0, 0 },
    { 0, 1, 8, 8, 1, 1, 8, 1, 0, 1, 2, 1, 8, 8, 1, 0 },
    { 0, 0, 1, 1, 1, 2, 1, 8, 1, 8, 1, 1, 0, 0, 0, 0 },
    { 0, 8, 1, 8, 8, 1, 8, 1, 1, 8, 8, 1, 1, 0, 8, 0 },
    { 0, 1, 8, 0, 0, 8, 0, 8, 1, 8, 0, 8, 8, 1, 0, 0 },
    { 1, 8, 0, 0, 8, 0, 0, 0, 1, 0, 0, 0, 0, 8, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 8, 0, 0, 0, 8, 0 }
};

// We cast them to 1D pointers so LCD_Draw_Sprite can read them sequentially
const uint8_t* const explosion_frame[3] = {
    (const uint8_t*)explosion_frame1,
    (const uint8_t*)explosion_frame2,
    (const uint8_t*)explosion_frame3
};

#endif