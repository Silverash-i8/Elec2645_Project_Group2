#include "Game_3.h"
#include "InputHandler.h"
#include "Menu.h"
#include <math.h>
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
#include "Game3Menu/Game3_Menu.h"
#include "POWERUp/PowerUp.h"

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
    Enemy* enemies = Enemy3_GetPool();
    // Read joystick
    Joystick_t joy_data;
    Joystick_Read(&joystick_cfg, &joy_data);

    // Move player
    Player_Move(joy_data.coord_mapped.x, -joy_data.coord_mapped.y); // Invert Y so down moves down on screen

    // Push player out of obstacles
    Obstacle* obs = Map3_GetObstacles();
    for (int k = 0; k < MAX_OBSTACLES; k++) {
        if (obs[k].active) {
            float odx = player.x - obs[k].x;
            float ody = player.y - obs[k].y;
            float odist = sqrtf(odx*odx + ody*ody);
            // Player sprite is rendered at PLAYER_SIZE*3 pixels wide, so visual radius = PLAYER_SIZE*3/2
            // Obstacle is drawn as a square of side obs.size, so visual radius = obs.size/2
            float min_dist = (float)(PLAYER_SIZE * 3) / 2.0f + (float)obs[k].size / 2.0f;
            if (odist < min_dist && odist > 0.01f) {
                float push = min_dist - odist;
                player.x += (odx / odist) * push;
                player.y += (ody / odist) * push;
            }
        }
    }

    // Update camera to follow player
    Camera_Init(player.x, player.y);

    // Auto-shoot the nearest enemy
    Spawning_AutoShootNearestEnemy(current_time);

    // Update bullets
    Bullet3_Update();

    // Update enemies
    Enemy3_Update(player.x, player.y);


    // Check bullet-enemy collisions
    Bullet* bullets = Bullet3_GetPool();
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].active) {
                    if (Collision_CheckCircle(bullets[i].x, bullets[i].y, 2,
                                              enemies[j].x, enemies[j].y, enemies[j].size)) {
                        float dead_x = enemies[j].x;
                        float dead_y = enemies[j].y;
                        Enemy3_TakeDamage(j, bullets[i].damage);
                        bullets[i].active = false;
                        Player_AddScore(10);
                        Game3_PlayHitSound(current_time);
                        // 30% chance to drop a power-up when enemy is killed
                        if (!enemies[j].active && (rand() % 10) < 3) {
                            PowerUp_Spawn(dead_x, dead_y);
                        }
                        break;
                    }
                }
            }
        }
    }

    // Check bullet-obstacle collisions
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (int k = 0; k < MAX_OBSTACLES; k++) {
                if (obs[k].active) {
                    if (Collision_CheckCircle(bullets[i].x, bullets[i].y, 2,
                                              obs[k].x, obs[k].y, obs[k].size)) {
                        bullets[i].active = false;
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
    // Update power-ups (pickup collision)
    PowerUp_Update();
    // Expire timed power-up boosts
    Player_UpdateTimers(current_time);
    Game3_UpdateBuzzer(current_time);
}

void render_game() {
    Render_ClearScreen();
    Render_Map(camera_x, camera_y);
    Render_Obstacles();
    Render_Player();
    Render_Bullets();
    Render_Enemies();
    Render_PowerUps();
    Render_UI();
    Render_Present();
}

MenuState Game3_Run(void) {
    // 1. Initialize systems quietly
    Player_Init();
    Camera_Init(player.x, player.y);
    Bullet3_Init();
    Enemy3_Init();
    Map3_Init();
    PowerUp_Init();

    // 2. SHOW THE MAIN MENU
    Game3_ShowMainMenu();

    // 3. START THE ACTUAL GAME (Only do this ONCE)
    game_timer = HAL_GetTick();
    Spawning_SpawnEnemyWave(0);

    // Initial start sound
    buzzer_tone(&buzzer_cfg, 1000, 50);
    HAL_Delay(100);
    buzzer_off(&buzzer_cfg);

    MenuState exit_state = MENU_STATE_HOME;

    // Game loop
    while (1) {
        uint32_t frame_start = HAL_GetTick();

        // Read input
        Input_Read();

        // Check death condition
        if (GameState_IsGameOver()) {
            // FIX: Turn off buzzer and show the screen before breaking!
            buzzer_off(&buzzer_cfg);
            Game3_ShowGameOver(); 
            
            exit_state = MENU_STATE_HOME;
            break;
        }

        // Dash Logic
        if (current_input.btn3_pressed) {
            Joystick_t dash_joy;
            Joystick_Read(&joystick_cfg, &dash_joy);
            Player_StartDash(dash_joy.coord_mapped.x, -dash_joy.coord_mapped.y);
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
