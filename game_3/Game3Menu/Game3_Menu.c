#include "Game3_Menu.h"
#include "LCD.h"              // Match your friend's include
#include "InputHandler.h"
#include "RENDER/Render.h"
#include "PLAYER/Player.h"    // So we can access player.score
#include "stm32l4xx_hal.h"
#include "Buzzer.h"
#include <stdio.h>            // For sprintf

extern ST7789V2_cfg_t cfg0;
extern Buzzer_cfg_t buzzer_cfg;

void Game3_ShowMainMenu(void) {
    bool in_main_menu = true;
    
    while (in_main_menu) {
        Input_Read();
        if (current_input.btn3_pressed) {
            in_main_menu = false;
        }

        LCD_Fill_Buffer(0); // Black background

        // ── TOP RED BANNER ──────────────────────────────────────────
        LCD_Draw_Rect(0, 0, 240, 88, 2, 1);

        // "ZOMBIE" yellow size 3 — 6 chars × 18px = 108px, centred x = (240-108)/2 = 66
        LCD_printString("ZOMBIE", 66, 10, 6, 3);
        // "SURVIVOR" white size 2 — 8 chars × 12px = 96px, centred x = (240-96)/2 = 72
        LCD_printString("SURVIVOR", 72, 57, 1, 2);

        // ── ORANGE DIVIDER ───────────────────────────────────────────
        LCD_Draw_Rect(0, 88, 240, 6, 5, 1);

        // ── ZOMBIE ICON ROW ──────────────────────────────────────────
        // 7 green zombie heads with white eyes evenly spaced
        for (int i = 0; i < 7; i++) {
            int cx = 17 + i * 34;
            LCD_Draw_Circle(cx, 115, 11, 3, 1);     // Green head
            LCD_Draw_Circle(cx - 4, 111, 2, 1, 1);  // Left eye (white)
            LCD_Draw_Circle(cx + 4, 111, 2, 1, 1);  // Right eye (white)
        }

        // Thin grey separator line
        LCD_Draw_Line(20, 134, 220, 134, 13);

        // ── CONTROLS BOX ─────────────────────────────────────────────
        LCD_Draw_Rect(15, 140, 210, 42, 13, 0); // Grey outline
        // "MOVE: JOYSTICK" white — 14 chars × 6 = 84px, in box: 15+(210-84)/2 = 78
        LCD_printString("MOVE: JOYSTICK", 78, 148, 1, 1);
        // Two-column layout for second row
        LCD_printString("SHOOT: AUTO", 25, 162, 14, 1);    // Cyan, left
        LCD_printString("DASH: BTN3", 155, 162, 5, 1);     // Orange, right

        // ── START BUTTON ─────────────────────────────────────────────
        LCD_Draw_Rect(25, 190, 190, 34, 5, 0);   // Orange outer border
        LCD_Draw_Rect(27, 192, 186, 30, 6, 0);   // Yellow inner border
        // "PRESS BTN3 TO START" yellow — 19 chars × 6 = 114px, centred: 25+(190-114)/2 = 63
        LCD_printString("PRESS BTN3 TO START", 63, 200, 6, 1);

        // ── FOOTER ───────────────────────────────────────────────────
        LCD_Draw_Circle(12, 230, 7, 2, 1);        // Red left dot
        LCD_Draw_Line(24, 230, 216, 230, 13);     // Grey connecting line
        LCD_Draw_Circle(228, 230, 7, 2, 1);       // Red right dot

        LCD_Refresh(&cfg0);
        HAL_Delay(30);
    }
}

void Game3_ShowGameOver(void) {
    buzzer_off(&buzzer_cfg);

    // Wait for player to release button before entering the screen
    do {
        Input_Read();
        HAL_Delay(10);
    } while (current_input.btn3_pressed);

    bool in_game_over = true;
    
    while (in_game_over) {
        Input_Read();
        if (current_input.btn3_pressed) {
            in_game_over = false;
        }

        LCD_Fill_Buffer(0); // Black background

        // ── RED EDGE FRAME ────────────────────────────────────────────
        LCD_Draw_Rect(0,   0,   240, 6,   2, 1); // Top
        LCD_Draw_Rect(0,   234, 240, 6,   2, 1); // Bottom
        LCD_Draw_Rect(0,   0,   6,   240, 2, 1); // Left
        LCD_Draw_Rect(234, 0,   6,   240, 2, 1); // Right
        // Inner grey border
        LCD_Draw_Rect(8, 8, 224, 224, 13, 0);

        // ── TITLE BANNER ─────────────────────────────────────────────
        LCD_Draw_Rect(10, 10, 220, 50, 2, 1); // Red banner fill
        // "GAME OVER" white size 3 — 9 chars × 18px = 162px, centred x = (240-162)/2 = 39
        LCD_printString("GAME OVER", 39, 22, 1, 3);

        // Orange divider below title
        LCD_Draw_Rect(10, 60, 220, 4, 5, 1);

        // ── SCORE BOX ────────────────────────────────────────────────
        LCD_Draw_Rect(20, 72, 200, 62, 13, 0); // Grey outline box

        char score_msg[24];
        sprintf(score_msg, "SCORE: %d", player.score);
        // "SCORE: XXXX" ~11 chars × 12px = 132px at size 2, centred x = (240-132)/2 = 54
        LCD_printString(score_msg, 54, 83, 6, 2);  // Yellow

        char wave_msg[24];
        sprintf(wave_msg, "WAVE REACHED: %d", player.wave);
        // ~15 chars × 6px = 90px at size 1, centred x = (240-90)/2 = 75
        LCD_printString(wave_msg, 75, 115, 14, 1); // Cyan

        // ── MESSAGE BOX ──────────────────────────────────────────────
        LCD_Draw_Rect(20, 147, 200, 52, 2, 0);  // Red outer border
        LCD_Draw_Rect(22, 149, 196, 48, 2, 0);  // Red inner border
        // "YOU WERE OVERRUN!" white — 17 chars × 6 = 102px, in box: 20+(200-102)/2 = 69
        LCD_printString("YOU WERE OVERRUN!", 69, 159, 1, 1);
        // "PRESS BTN3 TO EXIT" orange — 18 chars × 6 = 108px, in box: 20+(200-108)/2 = 66
        LCD_printString("PRESS BTN3 TO EXIT", 66, 175, 5, 1);

        // ── SKULL CORNER DECORATIONS ─────────────────────────────────
        LCD_Draw_Circle(22,  215, 13, 2, 1); // Red outer
        LCD_Draw_Circle(22,  215,  5, 0, 1); // Black hollow centre (eye socket)
        LCD_Draw_Circle(218, 215, 13, 2, 1);
        LCD_Draw_Circle(218, 215,  5, 0, 1);

        LCD_Refresh(&cfg0);
        HAL_Delay(30);
    }
}