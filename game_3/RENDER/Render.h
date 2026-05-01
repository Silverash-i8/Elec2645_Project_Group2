#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

// Render functions
void Render_Init(void);
void Render_ClearScreen(void);
void Render_Map(float camera_x, float camera_y);
void Render_Player(void);
void Render_Bullets(void);
void Render_Enemies(void);
void Render_UI(void);
void Render_Present(void);

#endif // RENDER_H