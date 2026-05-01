#include "Bullets.h"
#include "../CAMERA/Camera.h"

// bullet pool to manage active/inactive bullets without dynamic memory allocation
static Bullet bullet_pool[MAX_BULLETS];

void Bullet3_Init(void) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullet_pool[i].active = false;
    }
}

void Bullet3_Spawn(float x, float y, float vx, float vy, float damage) {
    // Looks for an inactive bullet slot to reuse
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullet_pool[i].active) {
            bullet_pool[i].x = x;
            bullet_pool[i].y = y;
            bullet_pool[i].vx = vx;
            bullet_pool[i].vy = vy;
            bullet_pool[i].damage = damage;
            bullet_pool[i].active = true;
            return;
        }
    }
}

void Bullet3_Update(void) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        // Only process bullets that are active
        if (bullet_pool[i].active) {
            // Move bullet based on its velocity
            bullet_pool[i].x += bullet_pool[i].vx;
            bullet_pool[i].y += bullet_pool[i].vy;

            // Deactivate bullet if it leaves an extended camera area
            if (!Camera_IsOnScreen(bullet_pool[i].x, bullet_pool[i].y, 40)) {
                bullet_pool[i].active = false;
            }
        }
    }
}

Bullet* Bullet3_GetPool(void) {
    return bullet_pool;
}