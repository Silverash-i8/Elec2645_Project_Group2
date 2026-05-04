#include "Enemy.h"
#include "../MAP/Map.h"
#include <math.h>
#include <stdlib.h>

static Enemy enemy_pool[MAX_ENEMIES];

void Enemy3_Init(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemy_pool[i].active = false;
    }
}

void Enemy3_Spawn(float x, float y, EnemyType type) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemy_pool[i].active) {
            enemy_pool[i].active = true;
            enemy_pool[i].x = x;
            enemy_pool[i].y = y;
            enemy_pool[i].type = type;

            // Stats are assigned automatically based on the type
            switch(type) {
                case SHAMBLER:
                    enemy_pool[i].hp = 2.0f;
                    enemy_pool[i].max_hp = 2.0f;
                    enemy_pool[i].speed = 2.0f;
                    enemy_pool[i].damage = 10.0f;
                    enemy_pool[i].size = 8;
                    break;
                case VECTOR:
                    enemy_pool[i].hp = 1.0f;
                    enemy_pool[i].max_hp = 1.0f;
                    enemy_pool[i].speed = 4.2f;
                    enemy_pool[i].damage = 5.0f;
                    enemy_pool[i].size = 8;
                    break;
                case TANK:
                    enemy_pool[i].hp = 10.0f;
                    enemy_pool[i].max_hp = 10.0f;
                    enemy_pool[i].speed = 0.5f;
                    enemy_pool[i].damage = 25.0f;
                    enemy_pool[i].size = 10;
                    break;
                case PATIENT_ZERO:
                    enemy_pool[i].hp = 100.0f;
                    enemy_pool[i].speed = 0.6f;
                    enemy_pool[i].damage = 40.0f;
                    enemy_pool[i].max_hp = 100.0f;
                    enemy_pool[i].size = 24; 
                    break;    
            }
            return; 
        }
    }
}

void Enemy3_Update(float playerX, float playerY) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemy_pool[i].active) {
            float dx = playerX - enemy_pool[i].x;
            float dy = playerY - enemy_pool[i].y;
            float distance = sqrtf(dx*dx + dy*dy);

            if (distance > 0.1f) { 
                enemy_pool[i].x += (dx / distance) * enemy_pool[i].speed;
                enemy_pool[i].y += (dy / distance) * enemy_pool[i].speed;
            }

            // Push enemy out of obstacles so they path around them
            Obstacle* obstacles = Map3_GetObstacles();
            for (int k = 0; k < MAX_OBSTACLES; k++) {
                if (obstacles[k].active) {
                    float odx = enemy_pool[i].x - obstacles[k].x;
                    float ody = enemy_pool[i].y - obstacles[k].y;
                    float odist = sqrtf(odx*odx + ody*ody);
                    // Shambler uses a 38x38 sprite so its visual radius is 19.
                    // Other enemies are drawn as circles with enemy.size as radius.
                    float enemy_radius = (enemy_pool[i].type == SHAMBLER) ? 19.0f : (float)enemy_pool[i].size;
                    float min_dist = enemy_radius + (float)obstacles[k].size / 2.0f;
                    if (odist < min_dist && odist > 0.01f) {
                        float push = min_dist - odist;
                        enemy_pool[i].x += (odx / odist) * push;
                        enemy_pool[i].y += (ody / odist) * push;
                    }
                }
            }
        }
    }
}

Enemy* Enemy3_GetPool(void) {
    return enemy_pool;
}

void Enemy3_TakeDamage(int index, float damage) {
    if (index >= 0 && index < MAX_ENEMIES && enemy_pool[index].active) {
        enemy_pool[index].hp -= damage;
        if (enemy_pool[index].hp <= 0) {
            enemy_pool[index].active = false;
        }
    }
}