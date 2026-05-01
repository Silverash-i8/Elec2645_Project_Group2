#include "Map.h"
#include "LCD.h"
#include "../CAMERA/Camera.h"
#include <math.h>
#include <stdint.h>

void Map3_Init(void) {
    // No persistent map state needed for simple grass background
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
