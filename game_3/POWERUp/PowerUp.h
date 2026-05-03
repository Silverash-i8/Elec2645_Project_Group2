#pragma once
#include <stdbool.h>
#include <stdint.h>

#define MAX_POWERUPS 5

typedef enum {
    POWERUP_HEALTH,
    POWERUP_SPEED,
    POWERUP_RAPID_FIRE
} PowerUpType;

typedef struct {
    float x, y;
    PowerUpType type;
    bool active;
    float size;
} PowerUp;

// Function blueprints
void PowerUp_Init(void);
void PowerUp_Spawn(float x, float y);
void PowerUp_Update(void);
PowerUp* PowerUp_GetPool(void);