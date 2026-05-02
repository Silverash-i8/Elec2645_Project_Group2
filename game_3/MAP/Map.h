#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdbool.h>

#define MAP_TILE_SIZE 20
#define MAX_OBSTACLES 30 // 30 obstacles scattered around

// The structure for our rocks/crates
typedef struct {
    float x, y;
    int size;
    bool active;
} Obstacle;

void Map3_Init(void);
void Map3_Draw(float camera_x, float camera_y);
uint8_t Map3_GetTileColor(int tile_x, int tile_y);

// NEW: Let other files access the obstacles
Obstacle* Map3_GetObstacles(void);

#endif // MAP_H