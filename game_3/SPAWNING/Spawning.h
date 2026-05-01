#ifndef SPAWNING_H
#define SPAWNING_H

#include <stdint.h>

// Spawning functions
void Spawning_Init(void);
void Spawning_SpawnEnemyWave(int wave);
int Spawning_FindNearestEnemyIndex(float x, float y);
void Spawning_AutoShootNearestEnemy(uint32_t current_time);

#endif // SPAWNING_H