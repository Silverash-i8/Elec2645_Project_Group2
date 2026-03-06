#ifndef SCORE_H
#define SCORE_H

#include "ST7789V2_Driver.h"

void Score_Init(void);
void Score_Reset(void);
void Score_Set(int value);
void Score_Add(int delta);
int  Score_Get(void);

// Draws the left-hand score panel into the screen buffer (does NOT refresh LCD)
// Call this before `LCD_Refresh(&cfg)` during your render loop.
void Score_Draw(ST7789V2_cfg_t* cfg);

#endif // SCORE_H
