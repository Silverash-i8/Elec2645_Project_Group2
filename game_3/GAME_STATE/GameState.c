#include "GameState.h"
#include "PLAYER/Player.h"
#include "Buzzer.h"
#include "../SPAWNING/Spawning.h"

extern Buzzer_cfg_t buzzer_cfg;

void GameState_Init(void) {
    // Game state initialization is handled in Player_Init
}

void GameState_Update(uint32_t current_time) {
    // Wave management
    if (GameState_ShouldSpawnWave(current_time)) {
        player.wave++;
        GameState_StartNewWave(player.wave);
    }
}

bool GameState_ShouldSpawnWave(uint32_t current_time) {
    return (current_time - player.wave_timer) > 10000; // New wave every 10 seconds
}

void GameState_StartNewWave(int wave_number) {
    player.wave = wave_number;
    player.wave_timer = HAL_GetTick();
    Spawning_SpawnEnemyWave(player.wave);
}

bool GameState_IsGameOver(void) {
    return !Player_IsAlive();
}