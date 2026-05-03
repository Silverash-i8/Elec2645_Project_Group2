#include "Explosion.h"
#include "explosion_sprite.h"
#include "stm32l4xx_hal.h"
#include "LCD.h"

#define FRAME_DELAY 200 // Time in ms between explosion frames

Explosion_t explosions[MAX_EXPLOSIONS];

void Explosions_Start(uint16_t x, uint16_t y) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) {
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].current_frame = 0;
            explosions[i].last_tick = HAL_GetTick();
            explosions[i].active = 1;
            break; // Found a slot, exit loop
        }
    }
}

void Explosions_Draw() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) continue;

        uint8_t frame_number = explosions[i].current_frame;
        uint8_t w, h;

        if (frame_number == 0) { w = EXPLOSION_FRAME1_WIDTH; h = EXPLOSION_FRAME1_HEIGHT; }   // First frame is smaller
        else if (frame_number == 1) { w = EXPLOSION_FRAME2_WIDTH; h = EXPLOSION_FRAME2_HEIGHT; }  // Second frame is medium
        else { w = EXPLOSION_FRAME3_WIDTH; h = EXPLOSION_FRAME3_HEIGHT; }  // Third frame is largest

        LCD_Draw_Sprite(explosions[i].x - (w/2), explosions[i].y - (h/2), w, h, explosion_frame[frame_number]);
    }
}

void Explosions_Update() {
    uint32_t current_time = HAL_GetTick();

    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) continue;

        // Check if it's time to move to the next frame
        if (current_time - explosions[i].last_tick >= FRAME_DELAY) {
            explosions[i].current_frame++;
            explosions[i].last_tick = current_time;

            // If we ran out of frames (0, 1, 2), kill the explosion
            if (explosions[i].current_frame >= 3) {
                explosions[i].active = 0;
            }
        }
    }
}

void Explosions_Clear() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        explosions[i].active = 0;
    }
}
