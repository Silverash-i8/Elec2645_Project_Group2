#include "Game_2.h"
#include "Menus/Tank_1990_Menu.h"
#include "Menus/Pause_Menu.h"
#include "TankEngine/TankEngine.h"
#include "Map/Map.h"
#include "Bullet/Explosion.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;
extern Buzzer_cfg_t buzzer_cfg;  // Buzzer control
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;
extern TIM_HandleTypeDef htim6;  // Timer handle for game frame timing

// Game engine instance
static TankEngine_t game_engine;

// Frame rate for this game (in milliseconds)
#define GAME2_FRAME_TIME_MS 33  // ~30 FPS

// Interrupt-driven timer flag (volatile for ISR communication)
volatile int g_game2_frame_flag = 0;

/**
 * @brief Calculate TIM6 period value from desired interrupt interval
 * @param interval_ms: Desired interrupt interval in milliseconds
 * @return uint32_t: Period value for TIM6 ARR register
 * 
 * TIM6 Configuration:
 *   - Prescaler: 7999 (divides 80MHz down to 10kHz)
 *   - Tick interval: 0.1ms
 *   - Period = (interval_ms × 10) - 1
 */
static uint32_t get_timer_period(uint32_t interval_ms)
{
    return (interval_ms * 10) - 1;
}

static void get_enemies_count_by_difficulty(TankEngine_t* engine, Start2MenuState difficulty) {
    // Difficulty determines how many/where enemies spawn
    // Easy: fewer enemies, Medium: more, Hard: more aggressive
    switch(difficulty) {
        case GAME2_MENU_EASY:  // Easy
            engine->num_enemies = 2;
            break;
        case GAME2_MENU_MEDIUM:  // Medium
            engine->num_enemies = 4;
            break;
        case GAME2_MENU_HARD:  // Hard
            engine->num_enemies = 6;
            break;
        default:
            engine->num_enemies = 4;  // Default to medium if something goes wrong
            break;  
    }
}

/**
 * @brief Display the game controls/instructions page
 */
void ShowInstructionsPage(void) {
    LCD_Fill_Buffer(0);
    LCD_printString("== CONTROLS ==", 30, 20, 4, 2);
    LCD_printString("Joystick", 20, 65, 2, 2);
    LCD_printString("- To Move Tank", 50, 90, 1, 1);
    
    LCD_printString("BreadBoard Button", 20, 120, 2, 2);
    LCD_printString("- To Fire", 50, 145, 1, 1);
    
    LCD_printString("Joystick Button", 20, 175, 2, 2);
    LCD_printString("- To Pause Game", 50, 200, 1, 1);
    
    LCD_Refresh(&cfg0);
    HAL_Delay(3000);  // Display for 3 seconds
}

/**
 * @brief Display the mission objectives and difficulty board
 */
void ShowMissionBoardPage(Start2MenuState difficulty) {
    LCD_Fill_Buffer(0);
    
    char level_text[32];
    char difficulty_name[16];
    if (difficulty == GAME2_MENU_EASY) {
        sprintf(difficulty_name, "EASY");
    } else if (difficulty == GAME2_MENU_MEDIUM) {
        sprintf(difficulty_name, "MEDIUM");
    } else {
        sprintf(difficulty_name, "HARD");
    }
    sprintf(level_text, "DIFFICULTY: %s", difficulty_name);
    
    LCD_printString("== MISSION ==", 28, 15, 4, 2);
    LCD_printString(level_text, 5, 65, 2, 2);
    LCD_printString("Objectives:", 25, 105, 2, 2);
    LCD_printString("- Destroy All Enemies", 30, 130, 1, 1);
    LCD_printString("- Protect Your Base", 30, 145, 1, 1);
    LCD_printString("- Keep All Units Alive", 30, 160, 1, 1);
    LCD_printString("Good Luck!", 50, 200, 2, 2);
    
    LCD_Refresh(&cfg0);
    HAL_Delay(3000);  // Display for 3 seconds
}

/**
 * @brief Display the mission accomplished victory page and wait for input
 */
void ShowMissionAccomplishedPage(void) {
    // Play victory melody (Victory/Success tone)
    buzzer_note(&buzzer_cfg, NOTE_G4, 50);
    HAL_Delay(100);
    buzzer_note(&buzzer_cfg, NOTE_C5, 50);
    HAL_Delay(100);
    buzzer_note(&buzzer_cfg, NOTE_E5, 50);
    HAL_Delay(100);
    buzzer_note(&buzzer_cfg, NOTE_G5, 50);
    HAL_Delay(200);
    buzzer_off(&buzzer_cfg);
    
    // Display winning page
    LCD_Fill_Buffer(0);
    LCD_printString("< VICTORY! >", 35, 15, 5, 2);
    LCD_printString("MISSION ACCOMPLISHED!", 2, 60, 1, 2);
    
    char final_score[32];
    sprintf(final_score, "FINAL SCORE: %d", TankEngine_GetScore(&game_engine));
    LCD_printString(final_score, 10, 105, 2, 2);
    
    // Display mission stats
    char enemies_text[32];
    sprintf(enemies_text, "Enemies Destroyed  : %d", TankEngine_GetScore(&game_engine) / 100);
    LCD_printString(enemies_text, 30, 140, 1, 1);
    
    char base_text[32];
    sprintf(base_text, "Base Survived      : %d", TankEngine_GetBaseHealth(&game_engine));
    LCD_printString(base_text, 30, 155, 1, 1);
    
    char lives_text[32];
    sprintf(lives_text, "Lives Remaining    : %d", TankEngine_GetLives(&game_engine));
    LCD_printString(lives_text, 30, 170, 1, 1);
    
    LCD_printString("Press Joystick BTN", 45, 225, 1, 1);
    
    LCD_Refresh(&cfg0);
    
    // Wait for Joystick button to be pressed
    while (1) {
        Input_Read();
        if (current_input.btn3_pressed) {
            break;
        }
        HAL_Delay(100);
    }
}

/**
 * @brief Display the game over page and wait for input
 */
void ShowGameOverPage(void) {
    // Play game over melody (Sad tone)
    buzzer_note(&buzzer_cfg, NOTE_C5, 70);
    HAL_Delay(200);
    buzzer_note(&buzzer_cfg, NOTE_A4, 70);
    HAL_Delay(200);
    buzzer_note(&buzzer_cfg, NOTE_G4, 70);
    HAL_Delay(300);
    buzzer_off(&buzzer_cfg);
    
    // Display game over page with improved text
    LCD_Fill_Buffer(0);
    
    // Title
    LCD_printString("< GAME OVER >", 35, 15, 5, 2);
    
    // Determine outcome message
    if (game_engine.base_health < 1) {
        LCD_printString("BASE DESTROYED!", 20, 60, 1, 2);
        LCD_printString("Mission Failed...", 20, 85, 1, 2);
    } else {
        LCD_printString("NO LIVES LEFT!", 25, 60, 1, 2);
        LCD_printString("All Units Down!", 25, 85, 1, 2);
    }
    
    // Display final score
    char final_score[32];
    sprintf(final_score, "FINAL SCORE: %d", TankEngine_GetScore(&game_engine));
    LCD_printString(final_score, 10, 125, 2, 2);
    
    // Display stats
    char lives_text[32];
    sprintf(lives_text, "Lives Used       : %d", 3 - TankEngine_GetLives(&game_engine));
    LCD_printString(lives_text, 30, 160, 1, 1);
    
    char base_text[32];
    sprintf(base_text, "Base Health      : %d", TankEngine_GetBaseHealth(&game_engine));
    LCD_printString(base_text, 30, 175, 1, 1);
    
    // Return instruction
    LCD_printString("Press Joystick BTN", 45, 225, 1, 1);
    LCD_Refresh(&cfg0);
    
    // Wait for Joystick button to be pressed
    while (1) {
        Input_Read();
        if (current_input.btn3_pressed) {
            break;
        }
        HAL_Delay(100);
    }
}
MenuState Game2_Run(void) {
    // Initialize menu system
    MenuSystem difficulty_menu;
    Game_Menu_Init(&difficulty_menu);
    
    // Show difficulty menu
    Start2MenuState selected_difficulty = Game_Menu_Run(&difficulty_menu);

    // Check if user quit
    if (selected_difficulty == GAME2_MENU_QUIT) {
            return MENU_STATE_HOME;  // Go back to main menu
    }
    
    // Clear any explosions from previous game
    Explosions_Clear();
    
    // Display instructions and mission pages
    ShowInstructionsPage();
    ShowMissionBoardPage(selected_difficulty);

    // Initialize the game engine

    // Initialize the map based on selected difficulty
    if (selected_difficulty == GAME2_MENU_EASY) {
        Map_Init_Easy();
    } else if (selected_difficulty == GAME2_MENU_MEDIUM) {
        Map_Init_Medium();
    } else if (selected_difficulty == GAME2_MENU_HARD) {
        Map_Init_Hard();
    }
    // Spawn enemies based on selected difficulty
    get_enemies_count_by_difficulty(&game_engine, selected_difficulty);
    
    // Start tank at bottom-left of map
    TankEngine_Init(&game_engine, 25, 220, TANK_SPEED, 0);  // 0 = facing up
 
    // Play startup melody (Mario Bros inspired)
    buzzer_note(&buzzer_cfg, NOTE_E5, 50);
    HAL_Delay(100);
    buzzer_note(&buzzer_cfg, NOTE_E5, 50);
    HAL_Delay(100);
    buzzer_note(&buzzer_cfg, NOTE_E5, 50);
    HAL_Delay(200);
    buzzer_note(&buzzer_cfg, NOTE_C5, 50);
    HAL_Delay(100);
    buzzer_note(&buzzer_cfg, NOTE_E5, 50);
    HAL_Delay(100);
    buzzer_note(&buzzer_cfg, NOTE_G5, 50);
    HAL_Delay(200);
    buzzer_off(&buzzer_cfg);

    // Configure and start game frame timer (TIM6)
    // Using interrupt-driven approach: CPU sleeps between frames
    htim6.Init.Period = get_timer_period(GAME2_FRAME_TIME_MS);
    HAL_TIM_Base_Init(&htim6);
    HAL_TIM_Base_Start_IT(&htim6);  // Start with interrupts enabled

    Start2MenuState exit_state = MENU_STATE_HOME;

    // Main game loop - waits for frame timer interrupt
    while (1) {
        
        // Read input
        Input_Read();
        
        // Read joystick for game control
        Joystick_Read(&joystick_cfg, &joystick_data);
        
        // Get input from joystick
        UserInput input = Joystick_GetInput(&joystick_data);

        // UPDATE: Game logic
        TankEngine_Update(&game_engine, input);
        
        // Handle firing with BT2
        if (current_input.btn2_pressed) {
            TankEngine_PlayerFire(&game_engine);
        }
        
        // Check if player won (all enemies destroyed)
        if (TankEngine_PlayerWon(&game_engine)) {
            // Clear any pending explosions
            Explosions_Clear();
            
            // Display mission accomplished page
            ShowMissionAccomplishedPage();
            
            exit_state = MENU_STATE_HOME;
            break;
        }
        // Check if game is over (player lost all lives or base destroyed)
        else if (TankEngine_IsGameOver(&game_engine)) {
            // Clear any pending explosions
            Explosions_Clear();
            
            // Display game over page
            ShowGameOverPage();
            
            exit_state = MENU_STATE_HOME;
            break;
        }

        // RENDER: Draw everything
        LCD_Fill_Buffer(0);
        
        // Draw game
        TankEngine_Draw(&game_engine);
        
        // Draw HUD (lives, score, base health)
        char hud_text[64];
        sprintf(hud_text, "Lives: %d | Score: %d  Base Health: %d", 
                TankEngine_GetLives(&game_engine), 
                TankEngine_GetScore(&game_engine),
                TankEngine_GetBaseHealth(&game_engine));
        LCD_printString(hud_text, 10, 6, 1, 1);
        
        // Display "Respawning" text only when tank was hit (freeze reason == 1)
        if (game_engine.freeze_reason == 1) {
            LCD_printString("Respawning...", 45, 110, 3, 2);
        }
        // Display "Base Destroyed" text when base is destroyed (freeze reason == 2)
        else if (game_engine.freeze_reason == 2) {
            LCD_printString("Base Destroyed!", 30, 110, 3, 2);
        }

        LCD_Refresh(&cfg0);
        
        // Check if user pressed Joystick button to pause game
        if (current_input.btn3_pressed) {
            // Initialize and show pause menu
            MenuSystem pause_menu;
            Pause_Menu_Init(&pause_menu);
            Pause_MenuState pause_choice = Pause_Menu_Run(&pause_menu);
            
            // Handle pause menu selection
            if (pause_choice == MENU_STATE_GAME2_MENU) {
                exit_state = MENU_STATE_GAME_2;
                break;  // Exit game loop
            }
            // If pause_choice == MENU_STATE_RESUME, continue the game loop
        }
        
        // Wait for frame timer interrupt
        // CPU sleeps in __WFI() until timer fires, saving power
        if (g_game2_frame_flag) {
            g_game2_frame_flag = 0;  // Clear flag for next frame
        } else {
            __WFI();  // Wait for interrupt - CPU sleeps, minimal power consumption
        }
    }
    
    // Stop the frame timer when exiting game
    HAL_TIM_Base_Stop_IT(&htim6);
    
    return exit_state;  // Tell main where to go next
}
