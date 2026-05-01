#include "Game_3.h"
#include "InputHandler.h"
#include "Menu.h"
#include "Joystick.h"
#include "PLAYER/Player.h"
#include "CAMERA/Camera.h"
#include "COLLISION/Collision.h"
#include "RENDER/Render.h"
#include "GAME_STATE/GameState.h"
#include "SPAWNING/Spawning.h"
#include "BULLETS/Bullets.h"
#include "ENEMY/Enemy.h"
#include "MAP/Map.h"
#include "Buzzer.h"
#include "PWM.h"
#include "stm32l4xx_hal.h"
#include <stdbool.h>

extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;      // LED PWM control
extern Buzzer_cfg_t buzzer_cfg; // Buzzer control
extern Joystick_cfg_t joystick_cfg;

/**
 * @brief Roguelike Survivor Game Implementation
 *
 * Features:
 * - Player movement with joystick
 * - Shooting with button
 * - Enemy waves
 * - Collision detection
 * - Health system
 * - Score tracking
 * - Infinite map with camera following
 */

// Frame rate for this game (in milliseconds) - fastest game
#define GAME3_FRAME_TIME_MS 16  // ~60 FPS (faster than others!)

static uint32_t game_timer = 0;
static bool buzzer_active = false;
static uint32_t buzzer_off_time = 0;

static void Game3_PlayHitSound(uint32_t current_time) {
    buzzer_tone(&buzzer_cfg, 1200, 20);
    buzzer_active = true;
    buzzer_off_time = current_time + 40;
}

static void Game3_UpdateBuzzer(uint32_t current_time) {
    if (buzzer_active && (int32_t)(current_time - buzzer_off_time) >= 0) {
        buzzer_off(&buzzer_cfg);
        buzzer_active = false;
    }
}

void update_game_logic(uint32_t current_time) {
    // Read joystick
    Joystick_t joy_data;
    Joystick_Read(&joystick_cfg, &joy_data);

    // Move player
    Player_Move(joy_data.coord_mapped.x, -joy_data.coord_mapped.y); // Invert Y so down moves down on screen

    // Update camera to follow player
    Camera_Update(player.x, player.y);

    // Auto-shoot the nearest enemy
    Spawning_AutoShootNearestEnemy(current_time);

    // Update bullets
    Bullet3_Update();

    // Update enemies
    Enemy3_Update(player.x, player.y);

    // Check bullet-enemy collisions
    Bullet* bullets = Bullet3_GetPool();
    Enemy* enemies = Enemy3_GetPool();
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].active) {
                    if (Collision_CheckCircle(bullets[i].x, bullets[i].y, 2,
                                              enemies[j].x, enemies[j].y, enemies[j].size)) {
                        Enemy3_TakeDamage(j, bullets[i].damage);
                        bullets[i].active = false;
                        Player_AddScore(10);
                        Game3_PlayHitSound(current_time);
                        break;
                    }
                }
            }
        }
    }

    // Check player-enemy collisions
    for (int j = 0; j < MAX_ENEMIES; j++) {
        if (enemies[j].active) {
            if (Collision_CheckCircle(player.x, player.y, PLAYER_SIZE,
                                      enemies[j].x, enemies[j].y, enemies[j].size)) {
                Player_TakeDamage(enemies[j].damage);
                enemies[j].active = false; // Enemy dies on contact
                Game3_PlayHitSound(current_time);
            }
        }
    }

    // Update game state (wave management)
    GameState_Update(current_time);
    Game3_UpdateBuzzer(current_time);
}

void render_game() {
    Render_ClearScreen();
    Render_Map(camera_x, camera_y);
    Render_Player();
    Render_Bullets();
    Render_Enemies();
    Render_UI();
    Render_Present();
}

MenuState Game3_Run(void) {
    // Initialize game state
    Player_Init();
    Camera_Init(player.x, player.y);
    game_timer = HAL_GetTick();

    // Initialize systems
    Bullet3_Init();
    Enemy3_Init();
    Map3_Init();

    // Spawn initial enemies around player
    Spawning_SpawnEnemyWave(0);

    // Play startup sound
    buzzer_tone(&buzzer_cfg, 1000, 50);
    HAL_Delay(100);
    buzzer_off(&buzzer_cfg);

    MenuState exit_state = MENU_STATE_HOME;

    // Game loop
    while (1) {
        uint32_t frame_start = HAL_GetTick();

        // Read input
        Input_Read();

        // Check exit condition
        if (current_input.btn3_pressed || GameState_IsGameOver()) {
            PWM_SetDuty(&pwm_cfg, 50);
            exit_state = MENU_STATE_HOME;
            break;
        }

        // Update game logic
        update_game_logic(frame_start);

        // Render
        render_game();

        // Frame timing
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < GAME3_FRAME_TIME_MS) {
            HAL_Delay(GAME3_FRAME_TIME_MS - frame_time);
        }
    }

    return exit_state;
}
