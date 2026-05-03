#include "Game3_Menu.h"
#include "LCD.h"              // Match your friend's include
#include "InputHandler.h"
#include "RENDER/Render.h"
#include "PLAYER/Player.h"    // So we can access player.score
#include "stm32l4xx_hal.h"
#include "Buzzer.h"
#include <stdio.h>            // For sprintf

extern ST7789V2_cfg_t cfg0;
extern Buzzer_cfg_t buzzer_cfg;

void Game3_ShowMainMenu(void) {
    bool in_main_menu = true;
    
    while (in_main_menu) {
        Input_Read();

        if (current_input.btn3_pressed) {
            in_main_menu = false;
        }

        // 1. Fill buffer with black (0)
        LCD_Fill_Buffer(0);
        
        // 2. Print Text: LCD_printString(text, x, y, color, size)
        // Using color 5 (usually Gold/Yellow) for Title, 1 (White) for others
        LCD_printString("ZOMBIE SURVIVOR", 20, 20, 5, 2);
        LCD_printString("PRESS BTN3 TO START", 25, 120, 1, 2);
        LCD_printString("DON'T GET BITTEN", 30, 180, 2, 1); // Red-ish color

        // 3. Refresh the screen
        LCD_Refresh(&cfg0);
        
        HAL_Delay(30); 
    }
}

void Game3_ShowGameOver(void) {
    buzzer_off(&buzzer_cfg); 

    // Wait for player to let go of dash button
    do {
        Input_Read();
        HAL_Delay(10);
    } while (current_input.btn3_pressed);

    bool in_game_over = true;
    
    while (in_game_over) {
        Input_Read();

        if (current_input.btn3_pressed) {
            in_game_over = false;
        }

        LCD_Fill_Buffer(0);
        
        // Game Over Text
        LCD_printString("GAME OVER", 20, 50, 2, 4); // Big Red text
        
        // Score Display
        char score_msg[20];
        sprintf(score_msg, "SCORE: %d", player.score);
        LCD_printString(score_msg, 80, 110, 1, 2);
        
        LCD_printString("PRESS BTN3 TO EXIT", 45, 180, 1, 1);
        
        LCD_Refresh(&cfg0);
        HAL_Delay(30); 
    }
}