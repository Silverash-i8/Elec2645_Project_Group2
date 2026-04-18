#include "Tank.h"
#include "Joystick.h"
#include "Map.h"
#include "LCD.h"
#include "green_tank_sprites.h"
#include <stdint.h>

void Tank_Init(Tank_t* tank, uint16_t x, uint16_t y, int16_t speed, uint8_t direction) {
    tank->x = x;
    tank->y = y;
    tank->speed = speed;
    tank->direction = direction;
    tank->alive = 1;
    tank->animation_frame = 0; // Start with IDLE frame
}
void Tank_Update(Tank_t* tank, UserInput input){
    if (!tank->alive) return; // Do not update if tank is destroyed

    uint8_t intended_direction = tank->direction;
    // Update direction based on input
    if (input.direction == N || input.direction == NE || input.direction == NW) {
        intended_direction = 0; // UP
    }
    else if (input.direction == W ) {
        intended_direction = 1; // RIGHT
    }
    else if (input.direction == S || input.direction == SE || input.direction == SW) {
        intended_direction = 2; // DOWN
    }
    else if (input.direction == E ) {
        intended_direction = 3; // LEFT
    }
    else {
        return;  // If there's no input, exit early
    }
    //Important concept from Tank 1990: the tank only changes direction when the joystick input changes, but does not move until the next update. This mimics the original game's mechanics.
    if (tank->direction != intended_direction) {
        // Change the facing direction, but DO NOT move this frame!
        tank->direction = intended_direction;
        return; 
    }

    // Calculate new position based on direction
    int16_t new_x = tank->x;
    int16_t new_y = tank->y;
    
    switch (tank->direction) {
        case 0: new_y -= tank->speed; break; // Up
        case 1: new_x += tank->speed; break; // Right
        case 2: new_y += tank->speed; break; // Down
        case 3: new_x -= tank->speed; break; // Left
    }
    
    // Check for collisions with map boundaries and solid tiles
    // Calculate tank bounding box
    int16_t tank_left_side   = new_x - TANK_SIZE / 2;
    int16_t tank_right_side  = new_x + TANK_SIZE / 2 - 1;   //-1 because pixel coordinates are 0-indexed
    int16_t tank_top_side    = new_y - TANK_SIZE / 2;
    int16_t tank_bottom_side = new_y + TANK_SIZE / 2 - 1;  //-1 because pixel coordinates are 0-indexed

    // 5. Screen boundary check
    if (tank_left_side < 2.5|| tank_right_side >= (MAP_WIDTH * TILE_SIZE -3) || tank_top_side < MAP_OFFSET_Y + 2.5 || tank_bottom_side >= MAP_OFFSET_Y + (MAP_HEIGHT * TILE_SIZE - 3)) {
        return; // Hit the edge of the screen map
    }

    // Convert to tile coordinates
    uint8_t left_col   = tank_left_side / TILE_SIZE;
    uint8_t right_col  = tank_right_side / TILE_SIZE;
    uint8_t top_row    = (tank_top_side - MAP_OFFSET_Y) / TILE_SIZE;
    uint8_t bottom_row = (tank_bottom_side - MAP_OFFSET_Y) / TILE_SIZE;

    // Check all 4 corners
    if (!Map_IsSolid(top_row, left_col) &&
        !Map_IsSolid(top_row, right_col) &&
        !Map_IsSolid(bottom_row, left_col) &&
        !Map_IsSolid(bottom_row, right_col))
    {
        tank->x = new_x;
        tank->y = new_y;
        // Animate tracks when moving
        tank->animation_frame = (tank->animation_frame + 1) % 2;
    }
}
void Tank_Draw(Tank_t* tank) {
    if (!tank->alive) return; // Do not draw if tank is destroyed
    
    // Calculate position for top-left corner of sprite
    int16_t x_pos = tank->x - GREEN_TANK_WIDTH / 2;
    int16_t y_pos = tank->y - GREEN_TANK_HEIGHT / 2;
    
    // Select sprite based on tank direction
    const uint8_t* sprite_data = NULL;
    uint16_t sprite_width = GREEN_TANK_WIDTH;
    uint16_t sprite_height = GREEN_TANK_HEIGHT;
    
    switch (tank->direction) {
        case 0: sprite_data = (const uint8_t*)green_tank_up_data; break;       // UP
        case 1: sprite_data = (const uint8_t*)green_tank_right_data; break;   // RIGHT
        case 2: sprite_data = (const uint8_t*)green_tank_down_data; break;    // DOWN
        case 3: sprite_data = (const uint8_t*)green_tank_left_data; break;    // LEFT
        default: sprite_data = (const uint8_t*)green_tank_up_data; break;
    }
    
    // Draw the tank sprite
    if (sprite_data != NULL) {
        LCD_Draw_Sprite(x_pos, y_pos, sprite_height, sprite_width, sprite_data);
    }
}