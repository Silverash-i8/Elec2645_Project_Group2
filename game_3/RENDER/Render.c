#include "Render.h"
#include "LCD.h"
#include "../MAP/Map.h"
#include "../BULLETS/Bullets.h"
#include "../ENEMY/Enemy.h"
#include "../PLAYER/Player.h"
#include "../CAMERA/Camera.h"
#include "../POWERUp/PowerUp.h"
#include "../Sprites/Players/player_up.h"
#include "../Sprites/Players/player_down.h"
#include "../Sprites/Players/player_left.h"
#include "../Sprites/Players/player_right.h"
#include "../Sprites/Enemies/Shamblers/shambler_left.h"
#include "../Sprites/Enemies/Shamblers/shambler_right.h"
#include "../Sprites/Enemies/Shamblers/shambler_default.h"
#include "../Sprites/Enemies/Vectors/vector_left.h"
#include "../Sprites/Enemies/Vectors/vector_right.h"
#include "../Sprites/Enemies/Tanks/tank_left.h"
#include "../Sprites/Enemies/Tanks/tank_right.h"
#include "../Sprites/Enemies/PatientZero/patient_zero_left.h"
#include "../Sprites/Enemies/PatientZero/patient_zero_right.h"
#include "../Sprites/PowerUps/powerup_health.h"
#include "../Sprites/PowerUps/powerup_speed.h"
#include "../Sprites/PowerUps/powerup_rapid_fire.h"
#include "../Sprites/Obstacles/obstacle.h"
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

            if (Camera_IsOnScreen(obstacles[i].x, obstacles[i].y, obstacles[i].size)) {
                LCD_Draw_Sprite_Scaled(
                    screen_x - (obstacles[i].size / 2),
                    screen_y - (obstacles[i].size / 2),
                    30, 30,
                    (const uint8_t *)obstacle_data,
                    obstacles[i].size / 30 > 0 ? obstacles[i].size / 30 : 1
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
                // --- VECTOR ---
                else if (enemies[i].type == VECTOR) {
                    const uint8_t *current_sprite = (enemies[i].x > player.x)
                        ? (const uint8_t *)vector_left_data
                        : (const uint8_t *)vector_right_data;
                    LCD_Draw_Sprite_Scaled(screen_x, screen_y, 16, 16, current_sprite, 2);
                }
                // --- TANK ---
                else if (enemies[i].type == TANK) {
                    const uint8_t *current_sprite = (enemies[i].x > player.x)
                        ? (const uint8_t *)tank_left_data
                        : (const uint8_t *)tank_right_data;
                    LCD_Draw_Sprite_Scaled(screen_x, screen_y, 20, 20, current_sprite, 2);
                }
                // --- PATIENT ZERO ---
                else if (enemies[i].type == PATIENT_ZERO) {
                    const uint8_t *current_sprite = (enemies[i].x > player.x)
                        ? (const uint8_t *)patient_zero_left_data
                        : (const uint8_t *)patient_zero_right_data;
                    LCD_Draw_Sprite_Scaled(screen_x, screen_y, 48, 48, current_sprite, 1);
                }
            }
        }
    }
}


void Render_PowerUps(void) {
    PowerUp* powerups = PowerUp_GetPool();
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            int screen_x, screen_y;
            Camera_WorldToScreen(powerups[i].x, powerups[i].y, &screen_x, &screen_y);
            if (Camera_IsOnScreen(powerups[i].x, powerups[i].y, (int)powerups[i].size)) {
                const uint8_t *sprite = NULL;
                switch (powerups[i].type) {
                    case POWERUP_HEALTH:     sprite = (const uint8_t *)powerup_health_data;     break;
                    case POWERUP_SPEED:      sprite = (const uint8_t *)powerup_speed_data;      break;
                    case POWERUP_RAPID_FIRE: sprite = (const uint8_t *)powerup_rapid_fire_data; break;
                    default: break;
                }
                if (sprite != NULL) {
                    LCD_Draw_Sprite_Scaled(screen_x - 8, screen_y - 8, 16, 16, sprite, 1);
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
