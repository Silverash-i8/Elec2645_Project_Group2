
#include "Game_1.h"
#include "Tetris_Config.h"
#include "Tetris_Pieces.h"
#include "Tetris_Logic.h"
#include "Tetris_Render.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "Buzzer.h"
#include "Score.h"
#include "ST7789V2_Driver.h"
#include "Joystick.h"
#include <stdio.h>
#include <stdlib.h>

extern ST7789V2_cfg_t cfg0;
extern Buzzer_cfg_t   buzzer_cfg;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t     joystick_data;

static uint32_t last_left = 0, last_right = 0, last_up = 0;

/* =========================================================
 *  RGB helper – cycles through palette indices over time
 * ========================================================= */
static const uint8_t rgb_cycle[] = {2, 5, 6, 3, 14, 15, 7}; /* R O Y G C M P */
#define RGB_CYCLE_LEN 7

static uint8_t rgb_color(uint32_t tick) {
    return rgb_cycle[(tick / 120) % RGB_CYCLE_LEN];
}

/* =========================================================
 *  Pause menu
 *  Returns 1 = continue, 0 = return to main menu
 * ========================================================= */
static int show_pause_menu(void) {
    /* Debounce – wait for BTN2 to be released before entering */
    do { Input_Read(); HAL_Delay(10); } while (current_input.btn2_pressed);

    int selected = 0; /* 0 = Continue, 1 = Main Menu */
    Direction last_dir = CENTRE;
    int choice = -1; /* -1 = still deciding */

    while (choice < 0) {
        Input_Read();
        Joystick_Read(&joystick_cfg, &joystick_data);
        UserInput joy = Joystick_GetInput(&joystick_data);
        Direction dir = joy.direction;

        /* Navigate with joystick up/down */
        if ((dir == N || dir == NW || dir == NE) && last_dir == CENTRE) {
            selected = 0;
        } else if ((dir == S || dir == SW || dir == SE) && last_dir == CENTRE) {
            selected = 1;
        }
        last_dir = (dir == CENTRE) ? CENTRE : dir;

        /* BTN3 confirms selection */
        if (current_input.btn3_pressed) {
            choice = selected;
        }

        uint32_t t = HAL_GetTick();
        uint8_t  rc = rgb_color(t);

        LCD_Fill_Buffer(0);

        /* Outer RGB border */
        LCD_Draw_Rect(0,   0,   240, 5,   rc, 1);
        LCD_Draw_Rect(0,   235, 240, 5,   rc, 1);
        LCD_Draw_Rect(0,   0,   5,   240, rc, 1);
        LCD_Draw_Rect(235, 0,   5,   240, rc, 1);

        /* Inner panel */
        LCD_Draw_Rect(10, 10, 220, 220, 13, 0);
        LCD_Draw_Rect(12, 12, 216, 80, 0, 1);

        /* Title */
        /* "PAUSED" = 6 chars x 6px x 3 = 108px -> x = (240-108)/2 = 66 */
        LCD_printString("PAUSED", 66, 22, rc, 3);
        LCD_Draw_Rect(12, 62, 216, 3, rc, 1);

        /* Subtitle */
        LCD_printString("Game is paused", 60, 72, 13, 1);

        /* Option boxes */
        /* Continue */
        uint8_t cont_col  = (selected == 0) ? rc : 13;
        uint8_t menu_col  = (selected == 1) ? rc : 13;

        LCD_Draw_Rect(30, 100, 180, 40, cont_col, 0);
        if (selected == 0) LCD_Draw_Rect(32, 102, 176, 36, 0, 1);
        /* "Continue" = 8 chars x 12px = 96px -> x = 30+(180-96)/2 = 72 */
        LCD_printString("Continue", 72, 112, (selected == 0) ? rc : 1, 2);

        /* Main Menu */
        LCD_Draw_Rect(30, 155, 180, 40, menu_col, 0);
        if (selected == 1) LCD_Draw_Rect(32, 157, 176, 36, 0, 1);
        /* "Main Menu" = 9 chars x 12px = 108px -> x = 30+(180-108)/2 = 66 */
        LCD_printString("Main Menu", 66, 167, (selected == 1) ? rc : 1, 2);

        /* Nav hint */
        LCD_printString("Joy up/down  BTN3=select", 20, 210, 13, 1);

        LCD_Refresh(&cfg0);
        HAL_Delay(GAME1_FRAME_TIME_MS);
    }

    /* Debounce exit */
    do { Input_Read(); HAL_Delay(10); } while (current_input.btn3_pressed || current_input.btn2_pressed);

    return (choice == 0) ? 1 : 0; /* 1 = resume, 0 = quit */
}

/* =========================================================
 *  Start screen
 * ========================================================= */
static void show_start_screen(void) {
    /* Wait for any held button to be released first */
    do { Input_Read(); HAL_Delay(10); } while (current_input.btn3_pressed);

    int running = 1;
    while (running) {
        Input_Read();
        if (current_input.btn3_pressed) running = 0;

        uint32_t t = HAL_GetTick();
        uint8_t  rc = rgb_color(t);

        LCD_Fill_Buffer(0);

        /* Top RGB banner */
        LCD_Draw_Rect(0, 0, 240, 8, rc, 1);
        LCD_Draw_Rect(0, 232, 240, 8, rc, 1);

        /* Tetris block art: colourful stack of mini-blocks as decoration */
        for (int col = 0; col < 10; col++) {
            uint8_t c = block_colors[(col % 7) + 1];
            LCD_Draw_Rect(20 + col * 20, 18, 16, 8, c, 1);
        }

        /* Title: size 4 → 6 chars × 6px × 4 = 144px → x = (240-144)/2 = 48 */
        LCD_printString("TETRIS", 48, 36, rc, 4);

        /* Subtitle decorative line */
        LCD_Draw_Rect(30, 76, 180, 3, rc, 1);

        /* Flavour text */
        LCD_printString("Stack.  Clear.  Survive.", 18, 90, 1, 1);

        /* Controls box */
        LCD_Draw_Rect(25, 110, 190, 68, rc, 0);
        LCD_Draw_Rect(27, 112, 186, 64, 13, 0);
        LCD_printString("CONTROLS", 79, 118, rc, 1);
        LCD_printString("Joystick  LEFT / RIGHT", 33, 132, 1, 1);
        LCD_printString("Joystick  UP  = rotate", 33, 144, 1, 1);
        LCD_printString("Joystick  DOWN = soft drop", 33, 156, 1, 1);
        LCD_printString("BTN3  =  hard drop", 33, 168, 1, 1);

        /* Blinking prompt – blink every 500 ms */
        if ((t / 500) % 2 == 0) {
            LCD_Draw_Rect(28, 192, 184, 26, rc, 0);
            LCD_printString("PRESS BTN3 TO START", 35, 199, rc, 1);
        }

        /* Bottom block row decoration */
        for (int col = 0; col < 10; col++) {
            uint8_t c = block_colors[((col + 3) % 7) + 1];
            LCD_Draw_Rect(20 + col * 20, 222, 16, 8, c, 1);
        }

        LCD_Refresh(&cfg0);
        HAL_Delay(GAME1_FRAME_TIME_MS);
    }
    /* Debounce */
    while (current_input.btn3_pressed) { Input_Read(); HAL_Delay(20); }
}

/* =========================================================
 *  Game-over screen
 * ========================================================= */
static void show_game_over_screen(int final_score) {
    /* Clear any held button */
    do { Input_Read(); HAL_Delay(10); } while (current_input.btn3_pressed);

    /* Play game-over melody while waiting */
    const Buzzer_Note_t melody[] = { NOTE_C5, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_F4, NOTE_E4 };
    const uint16_t m_dur[] = { 400, 300, 300, 300, 400, 400 };
    const int m_len = (int)(sizeof(melody) / sizeof(melody[0]));

    int stop = 0;
    while (!stop) {
        for (int idx = 0; idx < m_len; idx++) {
            buzzer_note(&buzzer_cfg, melody[idx], 45);
            uint32_t note_start = HAL_GetTick();
            while (HAL_GetTick() - note_start < m_dur[idx]) {
                Input_Read();
                if (current_input.btn3_pressed) { stop = 1; break; }

                /* Render game-over screen while melody plays */
                uint32_t t = HAL_GetTick();
                uint8_t  rc = rgb_color(t);
                LCD_Fill_Buffer(0);

                /* Outer RGB frame */
                LCD_Draw_Rect(0,   0,   240, 6,   rc, 1);
                LCD_Draw_Rect(0,   234, 240, 6,   rc, 1);
                LCD_Draw_Rect(0,   0,   6,   240, rc, 1);
                LCD_Draw_Rect(234, 0,   6,   240, rc, 1);

                /* Inner dark border */
                LCD_Draw_Rect(8, 8, 224, 224, 13, 0);

                /* Title banner */
                LCD_Draw_Rect(12, 12, 216, 48, 0, 1);
                LCD_printString("GAME OVER", 36, 20, rc, 3);

                /* Divider */
                LCD_Draw_Rect(12, 60, 216, 3, rc, 1);

                /* Score box */
                LCD_Draw_Rect(22, 72, 196, 58, 13, 0);
                char score_buf[24];
                snprintf(score_buf, sizeof(score_buf), "Score: %d", final_score);
                int slen = 0; while (score_buf[slen]) slen++;
                int sx = 22 + (196 - slen * 12) / 2;
                if (sx < 22) sx = 22;
                LCD_printString(score_buf, (uint16_t)sx, 82, 6, 2);

                /* Decorative broken-block row */
                for (int col = 0; col < 8; col++) {
                    uint8_t bc = block_colors[(col % 7) + 1];
                    LCD_Draw_Rect(30 + col * 22, 142, 18, 8, bc, 1);
                }

                /* Message box */
                LCD_Draw_Rect(22, 158, 196, 44, rc, 0);
                LCD_Draw_Rect(24, 160, 192, 40, 0, 1);
                LCD_printString("BLOCKS TOOK OVER!", 36, 166, 1, 1);
                LCD_printString("PRESS BTN3 TO EXIT", 34, 182, rc, 1);

                /* Corner diamonds using circles */
                LCD_Draw_Circle(22,  215, 8, rc, 1);
                LCD_Draw_Circle(22,  215, 3, 0, 1);
                LCD_Draw_Circle(218, 215, 8, rc, 1);
                LCD_Draw_Circle(218, 215, 3, 0, 1);

                LCD_Refresh(&cfg0);
                HAL_Delay(30);
            }
            buzzer_off(&buzzer_cfg);
            if (stop) break;
        }
        if (!stop) HAL_Delay(150);
    }

    /* Show screen once more while waiting for the final exit press */
    while (current_input.btn3_pressed) { Input_Read(); HAL_Delay(20); }
    while (1) {
        Input_Read();
        if (current_input.btn3_pressed) break;

        uint32_t t = HAL_GetTick();
        uint8_t  rc = rgb_color(t);
        LCD_Fill_Buffer(0);

        LCD_Draw_Rect(0,   0,   240, 6,   rc, 1);
        LCD_Draw_Rect(0,   234, 240, 6,   rc, 1);
        LCD_Draw_Rect(0,   0,   6,   240, rc, 1);
        LCD_Draw_Rect(234, 0,   6,   240, rc, 1);
        LCD_Draw_Rect(8, 8, 224, 224, 13, 0);
        LCD_Draw_Rect(12, 12, 216, 48, 0, 1);
        LCD_printString("GAME OVER", 36, 20, rc, 3);
        LCD_Draw_Rect(12, 60, 216, 3, rc, 1);
        LCD_Draw_Rect(22, 72, 196, 58, 13, 0);
        char score_buf[24];
        snprintf(score_buf, sizeof(score_buf), "Score: %d", final_score);
        int slen = 0; while (score_buf[slen]) slen++;
        int sx = 22 + (196 - slen * 12) / 2;
        if (sx < 22) sx = 22;
        LCD_printString(score_buf, (uint16_t)sx, 82, 6, 2);
        for (int col = 0; col < 8; col++) {
            uint8_t bc = block_colors[(col % 7) + 1];
            LCD_Draw_Rect(30 + col * 22, 142, 18, 8, bc, 1);
        }
        LCD_Draw_Rect(22, 158, 196, 44, rc, 0);
        LCD_Draw_Rect(24, 160, 192, 40, 0, 1);
        LCD_printString("BLOCKS TOOK OVER!", 36, 166, 1, 1);
        LCD_printString("PRESS BTN3 TO EXIT", 34, 182, rc, 1);
        LCD_Draw_Circle(22,  215, 8, rc, 1);
        LCD_Draw_Circle(22,  215, 3, 0, 1);
        LCD_Draw_Circle(218, 215, 8, rc, 1);
        LCD_Draw_Circle(218, 215, 3, 0, 1);
        LCD_Refresh(&cfg0);
        HAL_Delay(30);
    }
}

/* =========================================================
 *  Main game loop
 * ========================================================= */
MenuState Game1_Run(void) {
    /* Show start screen first */
    show_start_screen();

    /* Initialize grid to empty */
    for (int r = 0; r < TETRIS_ROWS; r++)
        for (int c = 0; c < TETRIS_COLS; c++)
            tetris_grid[r][c] = 0;

    /* Seed RNG and initialise block queue */
    srand(HAL_GetTick());
    Score_Reset();
    init_next_blocks();
    current_block = make_block_public(next_blocks[0].type);
    /* Shift queue down */
    for (int i = 0; i < NEXT_BLOCK_COUNT - 1; i++) next_blocks[i] = next_blocks[i + 1];
    next_blocks[NEXT_BLOCK_COUNT - 1] = make_block_public(random_block());

    /* Play startup sound */
    buzzer_tone(&buzzer_cfg, 1000, 30);
    HAL_Delay(50);
    buzzer_off(&buzzer_cfg);

    MenuState exit_state = MENU_STATE_HOME;

    Joystick_cfg_t* joy_cfg = &joystick_cfg;
    Joystick_t*     joy_data = &joystick_data;
    uint32_t last_fall = HAL_GetTick();
    int soft_drop   = 0;
    int block_landed = 0;

    while (1) {
        uint32_t frame_start = HAL_GetTick();

        Input_Read();
        Joystick_Read(joy_cfg, joy_data);
        UserInput joy = Joystick_GetInput(joy_data);

        uint32_t now = HAL_GetTick();

        /* Left */
        if ((joy.direction == W || joy.direction == NW || joy.direction == SW) && (now - last_left > JOY_REPEAT_DELAY)) {
            if (can_place(current_block.type, current_block.rotation, current_block.row, current_block.col - 1)) {
                current_block.col--;
                last_left = now;
            }
        } else if (joy.direction != W && joy.direction != NW && joy.direction != SW) {
            last_left = 0;
        }
        /* Right */
        if ((joy.direction == E || joy.direction == NE || joy.direction == SE) && (now - last_right > JOY_REPEAT_DELAY)) {
            if (can_place(current_block.type, current_block.rotation, current_block.row, current_block.col + 1)) {
                current_block.col++;
                last_right = now;
            }
        } else if (joy.direction != E && joy.direction != NE && joy.direction != SE) {
            last_right = 0;
        }
        /* Rotate */
        if (joy.direction == N && (now - last_up > JOY_REPEAT_DELAY)) {
            int new_rot = (current_block.rotation + 1) % 4;
            if (can_place(current_block.type, new_rot, current_block.row, current_block.col)) {
                current_block.rotation = new_rot;
                last_up = now;
            }
        } else if (joy.direction != N) {
            last_up = 0;
        }
        /* Soft drop */
        soft_drop = (joy.direction == S || joy.direction == SE || joy.direction == SW) ? 1 : 0;

        /* Pause – BTN2 (PC2) */
        if (current_input.btn2_pressed) {
            int resume = show_pause_menu();
            if (!resume) {
                exit_state = MENU_STATE_HOME;
                break;
            }
            /* Resume: reset fall timer so block doesn't instantly drop */
            last_fall = HAL_GetTick();
            last_left = last_right = last_up = 0;
        }

        if (current_input.btn3_pressed) {
            hard_drop();
            block_landed = 1;
        }

        /* --- BLOCK FALLING --- */
        uint32_t fall_interval = soft_drop ? 40 : 400;
        if (now - last_fall > fall_interval) {
            if (can_place(current_block.type, current_block.rotation, current_block.row + 1, current_block.col)) {
                current_block.row++;
            } else {
                lock_block();
                block_landed = 1;
            }
            last_fall = now;
        }

        /* --- RENDER --- */
        LCD_Fill_Buffer(0);

        /* Centred title with RGB colour */
        /* "TETRIS" = 6 chars × 6px × scale 3 = 108px → x = (240-108)/2 = 66 */
        const char *title = "TETRIS";
        uint8_t title_color = rgb_color(now);
        LCD_printString(title, 66, 8, title_color, 3);

        draw_tetris_grid();
        draw_shadow();

        draw_block(current_block.type, current_block.rotation,
                   current_block.row, current_block.col, 0);

        /* Next blocks label – centred over the 40px preview column */
        /* "Next:" = 5 chars × 6px = 30px → offset = (40-30)/2 = 5 */
        LCD_printString("Next:", NEXT_BLOCK_X + 5, NEXT_BLOCK_Y - 14, 1, 1);
        draw_next_block();

        /* Control hints – tucked in the left score-panel column */
        LCD_printString("B3=drop", 2, 213, 13, 1);
        LCD_printString("B2=pause", 2, 223, 13, 1);

        Score_Draw(&cfg0);

        /* Score popup */
        if (score_popup_active()) {
            score_popup_update();
        }

        LCD_Refresh(&cfg0);

        /* --- BLOCK LANDED: line-clear effects then next block --- */
        if (block_landed) {
            /* If lines were cleared (set by lock_block), do effects */
            if (last_lines_cleared > 0) {
                score_popup_init(last_lines_cleared * 100);
            }

            /* Advance block queue */
            current_block = next_blocks[0];
            current_block.row = 0;
            current_block.col = (TETRIS_COLS - 4) / 2;
            for (int i = 0; i < NEXT_BLOCK_COUNT - 1; i++) next_blocks[i] = next_blocks[i + 1];
            next_blocks[NEXT_BLOCK_COUNT - 1] = make_block_public(random_block());
            block_landed = 0;

            /* Game over check */
            if (!can_place(current_block.type, current_block.rotation,
                           current_block.row, current_block.col)) {
                show_game_over_screen(Score_Get());
                exit_state = MENU_STATE_HOME;
                break;
            }
        }

        /* Frame timing */
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < GAME1_FRAME_TIME_MS) {
            HAL_Delay(GAME1_FRAME_TIME_MS - frame_time);
        }
    }
    return exit_state;
}

