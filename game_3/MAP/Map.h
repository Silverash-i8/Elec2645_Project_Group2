#ifndef MAP_H
#define MAP_H

#include <stdint.h>

#define MAP_TILE_SIZE 20

void Map3_Init(void);
void Map3_Draw(float camera_x, float camera_y);
uint8_t Map3_GetTileColor(int tile_x, int tile_y);

#endif // MAP_H