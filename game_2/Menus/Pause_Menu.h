#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include <stdint.h>
#include "Menu.h"

// ==============================================
// PAUSE MENU STATE MACHINE
// ==============================================

typedef enum {
    MENU_STATE_PAUSE = 0,        // Pause Menu - select option
    MENU_STATE_RESUME,           // Resume game
    MENU_STATE_GAME2_MENU,        // Return to game 2 menu
} Pause_MenuState;

// ==============================================
// INITIALIZATION AND STATE MANAGEMENT
// ==============================================

/**
 * @brief Initialize the menu system
 */
void Pause_Menu_Init(MenuSystem* menu);

/**
 * @brief Run the menu - displays menu and waits for selection
 * 
 * Runs its own loop and returns the selected pause menu state.
 * 
 * @return Pause_MenuState - The pause menu option that was selected (RESUME or GAME2_MENU)
 */
Pause_MenuState Pause_Menu_Run(MenuSystem* menu);   
     
#endif // PAUSE_MENU_H