#ifndef BASE_EAGLE_H
#define BASE_EAGLE_H

#include <stdint.h>

#define BASE_EAGLE_FRAME_COUNT 1
#define BASE_EAGLE_FRAME_WIDTH 10
#define BASE_EAGLE_FRAME_HEIGHT 10

/* Piskel data for "Base eagle" */
const uint8_t base_eagle_data[10][10] = {
{2, 0, 0, 0, 0, 0, 0, 0, 0, 2},
{3, 5, 0, 1, 6, 3, 0, 0, 6, 3},
{2, 7, 2, 0, 4, 2, 0, 3, 6, 2},
{5, 5, 6, 5, 3, 3, 5, 5, 6, 5},
{1, 6, 2, 7, 6, 6, 5, 2, 7, 1},
{0, 5, 6, 5, 4, 3, 5, 6, 5, 0},
{0, 1, 6, 4, 3, 3, 5, 7, 1, 0},
{0, 0, 0, 0, 6, 6, 0, 0, 0, 0},
{0, 0, 4, 7, 6, 6, 8, 2, 0, 0},
{0, 0, 1, 1, 1, 1, 2, 1, 0, 0}};

#endif // BASE_EAGLE_H
