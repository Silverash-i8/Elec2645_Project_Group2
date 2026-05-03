#include "Render.h"
#include "LCD.h"
#include "../MAP/Map.h"
#include "../BULLETS/Bullets.h"
#include "../ENEMY/Enemy.h"
#include "../PLAYER/Player.h"
#include "../CAMERA/Camera.h"
#include "../Sprites/Players/player_up.h"
#include "../Sprites/Players/player_down.h"
#include "../Sprites/Players/player_left.h"
#include "../Sprites/Players/player_right.h"
#include "../Sprites/Enemies/Shamblers/shambler_left.h"
#include "../Sprites/Enemies/Shamblers/shambler_right.h"
#include "../Sprites/Enemies/Shamblers/shambler_default.h"
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
void Render_Obstacles(void) {
    Obstacle* obstacles = Map3_GetObstacles();
    
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            int screen_x, screen_y;
            Camera_WorldToScreen(obstacles[i].x, obstacles[i].y, &screen_x, &screen_y);

            // Only draw if the obstacle is currently on the screen
            if (Camera_IsOnScreen(obstacles[i].x, obstacles[i].y, obstacles[i].size)) {
                // Draw a solid gray/brown rectangle (adjust color index '8' to fit your palette)
                LCD_Draw_Rect(
                    screen_x - (obstacles[i].size/2), 
                    screen_y - (obstacles[i].size/2), 
                    obstacles[i].size, 
                    obstacles[i].size, 
                    3, // Outline color
                    1  // Fill flag (1 = filled)
                );
            }
        }
    }
}

void Render_Player(void) {
    const uint8_t* sprite_data = NULL;
    uint16_t base_size = 14; 
    uint8_t scale = 3;

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
        LCD_Draw_Sprite_Scaled(
            SCREEN_WIDTH/2 - (base_size * scale)/2,
            SCREEN_HEIGHT/2 - (base_size * scale)/2,
            base_size, 
            base_size, 
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

            // Using 38 as a safe camera buffer for your biggest enemies
            if (Camera_IsOnScreen(enemies[i].x, enemies[i].y, 38)) {
                
                // --- SHAMBLERS GET SPRITES ---
                if (enemies[i].type == SHAMBLER) {
                    const uint8_t *current_sprite; 
                    
                    if (enemies[i].x > player.x) {
                        current_sprite = (const uint8_t *)shambler_left_data;
                    } 
                    else if (enemies[i].x < player.x) {
                        current_sprite = (const uint8_t *)shambler_right_data;
                    } 
                    else {
                        current_sprite = (const uint8_t *)shambler_default_data;
                    }

                    LCD_Draw_Sprite_Scaled(
                        screen_x, 
                        screen_y, 
                        38, 38, 
                        current_sprite, 
                        1
                    );
                }
                // --- EVERYONE ELSE GETS CIRCLES ---
                else {
                    uint16_t color = 1; // Default color
                    
                    // Assign temporary colors so you can tell them apart!
                    if (enemies[i].type == VECTOR) color = 2;       // Color 2 (e.g., Green)
                    else if (enemies[i].type == TANK) color = 3;    // Color 3 (e.g., Red)
                    else if (enemies[i].type == PATIENT_ZERO) color = 4; // Color 4 (e.g., Purple)

                    // Draw the circle using the specific size from Enemy.c!
                    LCD_Draw_Circle(
                        screen_x, 
                        screen_y, 
                        enemies[i].size, // The radius you set for them
                        color,           // The color we just picked
                        1                // 1 = Filled circle
                    );
                }
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
