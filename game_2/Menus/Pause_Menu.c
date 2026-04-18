#include "Pause_Menu.h"
#include "LCD.h"
#include "InputHandler.h"
#include "Joystick.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;  // LCD configuration from main.c
extern Joystick_cfg_t joystick_cfg;  // Joystick configuration
extern Joystick_t joystick_data;     // Current joystick readings

// Pause menu options
static const char* menu_options[] = {
    "Resume",
    "Home"
};

#define NUM_MENU_OPTIONS 2

// Frame rate for menu (in milliseconds)
#define MENU_FRAME_TIME_MS 30  // ~33 FPS

/**
 * @brief Render the pause menu screen
 */
static void render_pause_menu(MenuSystem* menu) {
    LCD_Fill_Buffer(0);
    
    // Title
    LCD_printString("GAME PAUSED", 30, 30, 3, 3);
    
    // Menu options with selection highlight
    for (int i = 0; i < NUM_MENU_OPTIONS; i++) {
        uint16_t y_pos = 100 + (i * 40);
        uint8_t text_size = 2;
        
        if (i == menu->selected_option) {
            // Highlight selected option with inverted colors
            // Draw a rectangle around selected option
            // We'll use simple marker instead
            LCD_printString(">", 40, y_pos, 3, text_size);  // Arrow pointing to selection
        }
        
        LCD_printString((char*)menu_options[i], 70, y_pos, 1, text_size);
    }
    
    // Instructions
    LCD_printString("Press Joystick BTN to select", 40, 210, 1, 1);
    
    LCD_Refresh(&cfg0);
}

// ==============================================
// PUBLIC API IMPLEMENTATION
// ==============================================

void Pause_Menu_Init(MenuSystem* menu) {
    menu->selected_option = 0;
}

Pause_MenuState Pause_Menu_Run(MenuSystem* menu) {
    static Direction last_direction = CENTRE;  // Track last direction for debouncing
    Pause_MenuState selected_option = MENU_STATE_PAUSE;  // Which option was selected
    
    // Menu's own loop - runs until option is selected
    while (1) {
        uint32_t frame_start = HAL_GetTick();
        
        // Read input
        Input_Read();
        
        // Read current joystick position
        Joystick_Read(&joystick_cfg, &joystick_data);
        
        // Handle joystick navigation (up/down to select option)
        Direction current_direction = joystick_data.direction;
        
        if (current_direction == S && last_direction != S) {  // Joystick pushed DOWN
            // Move selection down
            menu->selected_option++;
            if (menu->selected_option >= NUM_MENU_OPTIONS) {
                menu->selected_option = 0;  // Wrap around
            }
        } 
        else if (current_direction == N && last_direction != N) {  // Joystick pushed UP
            // Move selection up
            if (menu->selected_option == 0) {
                menu->selected_option = NUM_MENU_OPTIONS - 1;  // Wrap around
            } else {
                menu->selected_option--;
            }
        }
        
        last_direction = current_direction;
        
        // Handle button press to select current option
        if (current_input.btn3_pressed) {
            // User pressed button - select the highlighted option
            if (menu->selected_option == 0) {
                selected_option = MENU_STATE_RESUME;
            } else if (menu->selected_option == 1) {
                selected_option = MENU_STATE_GAME2_MENU;
            }
            break;  // Exit menu loop - option selected!
        }
        
        // Render menu
        render_pause_menu(menu);
        
        // Frame timing - wait for remainder of frame time
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < MENU_FRAME_TIME_MS) {
            HAL_Delay(MENU_FRAME_TIME_MS - frame_time);
        }
    }
    
    return selected_option;  // Return which option was selected
}