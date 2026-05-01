#include "Render.h"
#include "LCD.h"
#include "../MAP/Map.h"
#include "../BULLETS/Bullets.h"
#include "../ENEMY/Enemy.h"
#include "../PLAYER/Player.h"
#include "../CAMERA/Camera.h"
#include "../Sprites/player_up.h"
#include "../Sprites/player_down.h"
#include "../Sprites/player_left.h"
#include "../Sprites/player_right.h"
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;

void Render_Init(void) {
    // Any initialization needed for rendering
}

void Render_ClearScreen(void) {
    LCD_Fill_Buffer(0); // Black background
}

void Render_Map(float camera_x, float camera_y) {
    Map3_Draw(camera_x, camera_y);
}

void Render_Player(void) {
    const uint8_t* sprite_data = NULL;
    uint16_t base_size = 14; 
    uint8_t scale = 3; // Increase this to 3 or 4 to make him even bigger!

    switch(player.direction) {
        case 0: // up
            sprite_data = (const uint8_t*)player_up_data;
            break;
        case 1: // right
            sprite_data = (const uint8_t*)player_right_data;
            break;
        case 2: // down
            sprite_data = (const uint8_t*)player_down_data;
            break;
        case 3: // left
            sprite_data = (const uint8_t*)player_left_data;
            break;
        default:
            sprite_data = (const uint8_t*)player_up_data;
            break;
    }

    if (sprite_data != NULL) {
        // Use the Scaled version of the draw function
        LCD_Draw_Sprite_Scaled(
            SCREEN_WIDTH/2 - (base_size * scale)/2,
            SCREEN_HEIGHT/2 - (base_size * scale)/2,
            base_size, // base rows
            base_size, // base columns
            sprite_data,
            scale      // The magic number that makes him bigger
        );
    }
}


void Render_Bullets(void) {
    Bullet* bullets = Bullet3_GetPool();
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            int screen_x, screen_y;
            Camera_WorldToScreen(bullets[i].x, bullets[i].y, &screen_x, &screen_y);

            // Only draw if on screen
            if (Camera_IsOnScreen(bullets[i].x, bullets[i].y, 10)) {
                LCD_Draw_Circle(screen_x, screen_y, 2, 3, 1); // Yellow bullets
            }
        }
    }
}

void Render_Enemies(void) {
    Enemy* enemies = Enemy3_GetPool();
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            int screen_x, screen_y;
            Camera_WorldToScreen(enemies[i].x, enemies[i].y, &screen_x, &screen_y);

            // Only draw if on screen
            if (Camera_IsOnScreen(enemies[i].x, enemies[i].y, enemies[i].size)) {
                uint8_t color;
                switch(enemies[i].type) {
                    case SHAMBLER: color = 1; break; // Red
                    case VECTOR: color = 4; break;   // Cyan
                    case TANK: color = 5; break;     // Magenta
                    case PATIENT_ZERO: color = 6; break; // Yellow
                }
                LCD_Draw_Circle(screen_x, screen_y, enemies[i].size/2, color, 1);
            }
        }
    }
}

void Render_UI(void) {
    char buffer[32];

    sprintf(buffer, "Health: %.0f", player.health);
    LCD_printString(buffer, 10, 10, 1, 1);

    sprintf(buffer, "Score: %d", player.score);
    LCD_printString(buffer, 10, 25, 1, 1);

    sprintf(buffer, "Wave: %d", player.wave);
    LCD_printString(buffer, 10, 40, 1, 1);

    // Health bar
    LCD_Draw_Rect(10, 55, 100, 10, 1, 0); // Outline
    int health_width = (int)(player.health / PLAYER_MAX_HEALTH * 98);
    if (health_width > 0) {
        LCD_Draw_Rect(11, 56, health_width, 8, 2, 1); // Green fill
    }
}

void Render_Present(void) {
    LCD_Refresh(&cfg0);
}