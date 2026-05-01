#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdint.h>
#include <stdbool.h>

// Game state functions
void GameState_Init(void);
void GameState_Update(uint32_t current_time);
bool GameState_ShouldSpawnWave(uint32_t current_time);
void GameState_StartNewWave(int wave_number);
bool GameState_IsGameOver(void);

#endif // GAME_STATE_H