#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
#include <stdbool.h>

// Screen dimensions
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

// Camera state
extern float camera_x;
extern float camera_y;

// Camera functions
void Camera_Init(float player_x, float player_y);
void Camera_WorldToScreen(float world_x, float world_y, int* screen_x, int* screen_y);
bool Camera_IsOnScreen(float world_x, float world_y, int size);

#endif // CAMERA_H