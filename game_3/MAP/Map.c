#include "Map.h"
#include "LCD.h"
#include "../CAMERA/Camera.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h> // Needed for rand()

// The static memory pool for your obstacles
static Obstacle obstacle_pool[MAX_OBSTACLES];

void Map3_Init(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacle_pool[i].active = true;
        
        if (i == 0) {
            // FORCE the first obstacle to spawn right in front of the player
            obstacle_pool[i].x = 40; 
            obstacle_pool[i].y = 40;
            obstacle_pool[i].size = 30; // Make it big
        } else {
            // Scatter the rest randomly but closer together (-500 to 500)
            obstacle_pool[i].x = (rand() % 1000) - 500; 
            obstacle_pool[i].y = (rand() % 1000) - 500;
            obstacle_pool[i].size = 15 + (rand() % 16); 
            
            if (fabs(obstacle_pool[i].x) < 50 && fabs(obstacle_pool[i].y) < 50) {
                obstacle_pool[i].x += 100;
            }
        }
    }
}

// Return the pool so the Renderer can draw them
Obstacle* Map3_GetObstacles(void) {
    return obstacle_pool;
}

void Map3_Draw(float camera_x, float camera_y) {
    const int grid_step = 40;
    const uint8_t grid_color = 15; // White grid

    float first_x = floorf(camera_x / grid_step) * grid_step;
    float first_y = floorf(camera_y / grid_step) * grid_step;
    float last_x = camera_x + SCREEN_WIDTH;
    float last_y = camera_y + SCREEN_HEIGHT;

    for (float x = first_x; x <= last_x; x += grid_step) {
        int screen_x = (int)(x - camera_x);
        if (screen_x >= 0 && screen_x <= SCREEN_WIDTH) {
            LCD_Draw_Line(screen_x, 0, screen_x, SCREEN_HEIGHT, grid_color);
        }
    }

    for (float y = first_y; y <= last_y; y += grid_step) {
        int screen_y = (int)(y - camera_y);
        if (screen_y >= 0 && screen_y <= SCREEN_HEIGHT) {
            LCD_Draw_Line(0, screen_y, SCREEN_WIDTH, screen_y, grid_color);
        }
    }
}