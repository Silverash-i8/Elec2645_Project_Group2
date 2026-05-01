#include "Collision.h"
#include <math.h>

bool Collision_CheckCircle(float x1, float y1, int size1, float x2, float y2, int size2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    float distance = sqrtf(dx*dx + dy*dy);
    return distance < (size1 + size2) / 2.0f;
}