#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_OBSTACLES 30 

typedef struct {
    float x, y;
    int size;
    bool active;
} Obstacle;

void Map3_Init(void);
void Map3_Draw(float camera_x, float camera_y);
Obstacle* Map3_GetObstacles(void);

#endif // MAP_H