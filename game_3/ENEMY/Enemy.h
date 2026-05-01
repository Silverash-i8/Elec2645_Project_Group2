#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_ENEMIES 20

typedef enum {
    SHAMBLER,
    VECTOR,
    TANK,
    PATIENT_ZERO
} EnemyType;

typedef struct {
    float x, y;
    float max_hp;
    int size;
    float hp;
    float speed;
    float damage;
    EnemyType type;
    bool active;
} Enemy;

void Enemy3_Init(void);
void Enemy3_Spawn(float x, float y, EnemyType type);
void Enemy3_Update(float playerX, float playerY);

// Get pointer to enemy pool for collision detection
Enemy* Enemy3_GetPool(void);

// Apply damage to enemy at index
void Enemy3_TakeDamage(int index, float damage);

#endif