#include "Spawning.h"
#include "../ENEMY/Enemy.h"
#include "../BULLETS/Bullets.h"
#include "../PLAYER/Player.h"
#include "Buzzer.h"
#include <math.h>
#include <stdlib.h>

extern Buzzer_cfg_t buzzer_cfg;

#define ENEMY_SPAWN_DISTANCE 250.0f

void Spawning_Init(void) {
    // Any initialization needed for spawning
}

void Spawning_SpawnEnemyWave(int wave) {
    int num_enemies = 3 + wave * 2; // Increase enemies per wave
    for (int i = 0; i < num_enemies; i++) {
        // Spawn in a circle around the player
        float angle = (float)rand() / RAND_MAX * 2 * M_PI;
        float distance = 150 + (float)rand() / RAND_MAX * 100; // Spawn 150-250 units away

        float spawn_x = player.x + cosf(angle) * distance;
        float spawn_y = player.y + sinf(angle) * distance;

        // Random enemy type (weighted towards easier ones)
        EnemyType type;
        int r = rand() % 100;
        if (r < 50) type = SHAMBLER;
        else if (r < 75) type = VECTOR;
        else if (r < 90) type = TANK;
        else type = PATIENT_ZERO;

        Enemy3_Spawn(spawn_x, spawn_y, type);
    }
}

int Spawning_FindNearestEnemyIndex(float x, float y) {
    Enemy* enemies = Enemy3_GetPool();
    int nearest = -1;
    float best_dist = 1e9f;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            continue;
        }
        float dx = enemies[i].x - x;
        float dy = enemies[i].y - y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < best_dist) {
            best_dist = dist;
            nearest = i;
        }
    }

    return nearest;
}

void Spawning_AutoShootNearestEnemy(uint32_t current_time) {
    int target = Spawning_FindNearestEnemyIndex(player.x, player.y);
    if (target < 0) {
        return;
    }

    if ((current_time - player.last_shot) <= 500) {
        return;
    }

    Enemy* enemies = Enemy3_GetPool();
    float dx = enemies[target].x - player.x;
    float dy = enemies[target].y - player.y;
    float dist = sqrtf(dx*dx + dy*dy);
    if (dist <= 0.1f) {
        return;
    }

    dx /= dist;
    dy /= dist;
    Bullet3_Spawn(player.x, player.y, dx * BULLET_SPEED, dy * BULLET_SPEED, BULLET_DAMAGE);
    player.last_shot = current_time;
}
