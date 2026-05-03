/**
 * @file Bullet.c
 * @brief Bullet implementation for Tank 1990 game
 */

#include "Bullet.h"
#include "LCD.h"
#include "../Map/Map.h"

void Bullet_Init(Bullet_t* bullet) {
    bullet->x = 0;
    bullet->y = 0;
    bullet->direction = 0;
    bullet->active = 0;
    bullet->owner_id = 0;
}

void Bullet_Fire(Bullet_t* bullet, uint16_t x, uint16_t y, uint8_t direction, uint8_t owner_id) {
    bullet->x = x;
    bullet->y = y;
    bullet->direction = direction;
    bullet->active = 1;
    bullet->owner_id = owner_id;
}

void Bullet_Update(Bullet_t* bullet) {
    if (!bullet->active) {
        return;
    }
    // Move bullet based on direction
    switch (bullet->direction) {
        case 0: bullet->y -= BULLET_SPEED; break; // Up
        case 1: bullet->x += BULLET_SPEED; break; // Right
        case 2: bullet->y += BULLET_SPEED; break; // Down
        case 3: bullet->x -= BULLET_SPEED; break; // Left
    }
    // Boundary check - deactivate if out of bounds
    if (bullet->x < 0 || bullet->x >= (MAP_WIDTH * TILE_SIZE) ||
        bullet->y < MAP_OFFSET_Y || bullet->y >= MAP_OFFSET_Y + (MAP_HEIGHT * TILE_SIZE)) {
        bullet->active = 0;
        return;
    }
    // Check collision with solid tiles (walls)
    uint8_t col = bullet->x / TILE_SIZE;
    uint8_t row = (bullet->y - MAP_OFFSET_Y) / TILE_SIZE;
    if (Map_IsSolid(row, col)) {
        // Destroy brick if hit
        Map_DestroyTile(row, col);
        bullet->active = 0;
        return;
    }
}

void Bullet_Draw(Bullet_t* bullet) {
    if (!bullet->active) {
        return;
    }

    // Draw bullet as a small white square
    LCD_Draw_Rect(bullet->x, bullet->y, BULLET_SIZE, BULLET_SIZE, 3, 1);
}