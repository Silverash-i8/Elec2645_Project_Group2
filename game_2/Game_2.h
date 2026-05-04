#ifndef GAME_2_H
#define GAME_2_H

#include "Menu.h"
#include "Menus/Tank_1990_Menu.h"

/**
 * @brief Game 2 - Tank 1990 Game Implementation
 * 
 * Main game entry point and page display functions
 */

/**
 * @brief Main game loop and initialization
 * @return MenuState - Where to go next (typically MENU_STATE_HOME for menu)
 */
MenuState Game2_Run(void);

/**
 * @brief Display the game controls/instructions page (3 seconds)
 */
void ShowInstructionsPage(void);

/**
 * @brief Display the mission objectives and difficulty board (3 seconds)
 * @param difficulty The selected difficulty level
 */
void ShowMissionBoardPage(Game2MenuState difficulty);

/**
 * @brief Display the mission accomplished victory page
 * Shows stats and waits for user input to return to menu
 */
void ShowMissionAccomplishedPage(void);

/**
 * @brief Display the game over page
 * Shows outcome and stats, waits for user input to return to menu
 */
void ShowGameOverPage(void);

#endif // GAME_2_H
