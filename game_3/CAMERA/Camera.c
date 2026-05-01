#include "Camera.h"
#include <stdbool.h>

// Camera state
float camera_x = 0;
float camera_y = 0;

void Camera_Init(float player_x, float player_y) {
    camera_x = player_x - SCREEN_WIDTH / 2;
    camera_y = player_y - SCREEN_HEIGHT / 2;
}

void Camera_Update(float player_x, float player_y) {
    // Update camera to follow player (center on screen)
    camera_x = player_x - SCREEN_WIDTH / 2;
    camera_y = player_y - SCREEN_HEIGHT / 2;
}

void Camera_WorldToScreen(float world_x, float world_y, int* screen_x, int* screen_y) {
    *screen_x = (int)(world_x - camera_x);
    *screen_y = (int)(world_y - camera_y);
}

bool Camera_IsOnScreen(float world_x, float world_y, int size) {
    int screen_x = (int)(world_x - camera_x);
    int screen_y = (int)(world_y - camera_y);

    return (screen_x >= -size && screen_x <= SCREEN_WIDTH + size &&
            screen_y >= -size && screen_y <= SCREEN_HEIGHT + size);
}