#ifndef TANK_1990_MENU_H
#define TANK_1990_MENU_H

#include <stdint.h>
#include "Menu.h"  // For MenuSystem definition

// ==============================================
// MENU STATE MACHINE
// ==============================================

typedef enum {
    GAME2_MENU_HOME = 0,        // Game Menu - select game
    GAME2_MENU_EASY,            // Easy Level
    GAME2_MENU_MEDIUM,          // Medium Level
    GAME2_MENU_HARD,            // Hard Level
    GAME2_MENU_QUIT,            // Quit Game
} Start2MenuState;

// Note: MenuSystem is defined in shared/Menu.h and reused here

// ==============================================
// INITIALIZATION AND STATE MANAGEMENT
// ==============================================

/**
 * @brief Initialize the menu system
 */
void Game_Menu_Init(MenuSystem* menu);

/**
 * @brief Run the menu - displays menu and waits for selection
 * 
 * Runs its own loop and returns the selected game state.
 * 
 * @return Start2MenuState - The game that was selected (GAME2_MENU_HOME, GAME2_MENU_EASY, GAME2_MENU_MEDIUM, GAME2_MENU_HARD, or GAME2_MENU_QUIT)
 */
Start2MenuState Game_Menu_Run(MenuSystem* menu);   

#endif // TANK_1990_MENU_H