
#include "Joystick.h"
#include "stm32l4xx_hal.h"
#include "Joystick.h"
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;


// Joystick input debounce
#define JOY_REPEAT_DELAY 150
#define JOY_FAST_REPEAT 60
static uint32_t last_left = 0, last_right = 0, last_up = 0;

#define TETRIS_ROWS 16
#define TETRIS_COLS 10
#define CELL_SIZE 10
#define GRID_WIDTH (TETRIS_COLS * CELL_SIZE)
#define GRID_HEIGHT (TETRIS_ROWS * CELL_SIZE)
#define GRID_ORIGIN_X 65  // Centered horizontally for 240x240 LCD
#define GRID_ORIGIN_Y 36  // Lowered to avoid title overlap
#define NEXT_BLOCK_X (GRID_ORIGIN_X + GRID_WIDTH + 18)
#define NEXT_BLOCK_Y (GRID_ORIGIN_Y + 20)

// Frame rate for this game (in milliseconds)
#define GAME1_FRAME_TIME_MS 30  // ~33 FPS

// Forward declarations for functions used before definition
static int can_place(int type, int rotation, int row, int col);
static void lock_block(void);
static void hard_drop(void);
static void draw_tetris_grid(void);


#include "Game_1.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "PWM.h"
#include "Buzzer.h"
#include "Score.h"
#include "ST7789V2_Driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;      // LED PWM control
extern Buzzer_cfg_t buzzer_cfg; // Buzzer control

// Block colors (LCD color indices)
static const uint8_t block_colors[8] = {0, 2, 3, 4, 5, 6, 7, 8};
// Tetris grid state: 0 = empty, 1-7 = block color
static uint8_t tetris_grid[TETRIS_ROWS][TETRIS_COLS];
// --- TETRIMINO DEFINITIONS ---
static const uint8_t tetriminos[7][4][4][4] = {
    // I
    {
        {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
        {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
        {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}
    },
    // J
    {
        {{2,0,0,0},{2,2,2,0},{0,0,0,0},{0,0,0,0}},
        {{0,2,2,0},{0,2,0,0},{0,2,0,0},{0,0,0,0}},
        {{0,0,0,0},{2,2,2,0},{0,0,2,0},{0,0,0,0}},
        {{0,2,0,0},{0,2,0,0},{2,2,0,0},{0,0,0,0}}
    },
    // L
    {
        {{0,0,3,0},{3,3,3,0},{0,0,0,0},{0,0,0,0}},
        {{0,3,0,0},{0,3,0,0},{0,3,3,0},{0,0,0,0}},
        {{0,0,0,0},{3,3,3,0},{3,0,0,0},{0,0,0,0}},
        {{3,3,0,0},{0,3,0,0},{0,3,0,0},{0,0,0,0}}
    },
    // O
    {
        {{0,4,4,0},{0,4,4,0},{0,0,0,0},{0,0,0,0}},
        {{0,4,4,0},{0,4,4,0},{0,0,0,0},{0,0,0,0}},
        {{0,4,4,0},{0,4,4,0},{0,0,0,0},{0,0,0,0}},
        {{0,4,4,0},{0,4,4,0},{0,0,0,0},{0,0,0,0}}
    },
    // S
    {
        {{0,5,5,0},{5,5,0,0},{0,0,0,0},{0,0,0,0}},
        {{0,5,0,0},{0,5,5,0},{0,0,5,0},{0,0,0,0}},
        {{0,0,0,0},{0,5,5,0},{5,5,0,0},{0,0,0,0}},
        {{5,0,0,0},{5,5,0,0},{0,5,0,0},{0,0,0,0}}
    },
    // T
    {
        {{0,6,0,0},{6,6,6,0},{0,0,0,0},{0,0,0,0}},
        {{0,6,0,0},{0,6,6,0},{0,6,0,0},{0,0,0,0}},
        {{0,0,0,0},{6,6,6,0},{0,6,0,0},{0,0,0,0}},
        {{0,6,0,0},{6,6,0,0},{0,6,0,0},{0,0,0,0}}
    },
    // Z
    {
        {{7,7,0,0},{0,7,7,0},{0,0,0,0},{0,0,0,0}},
        {{0,0,7,0},{0,7,7,0},{0,7,0,0},{0,0,0,0}},
        {{0,0,0,0},{7,7,0,0},{0,7,7,0},{0,0,0,0}},
        {{0,7,0,0},{7,7,0,0},{7,0,0,0},{0,0,0,0}}
    }
};


// Current and next block state
typedef struct {
    int type;      // 0-6
    int rotation;  // 0-3
    int row, col;  // top-left of 4x4 block in grid
} BlockState;
static BlockState current_block, next_block;


// Random block generator (returns 0-6)
static int random_block(void) {
    return rand() % 7;
}


// Draw a block (4x4) at grid position (row, col) with given type/rotation
static void draw_block(int type, int rotation, int row, int col, int shadow) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            uint8_t val = tetriminos[type][rotation][r][c];
            if (val) {
                uint16_t x = GRID_ORIGIN_X + (col + c) * CELL_SIZE;
                uint16_t y = GRID_ORIGIN_Y + (row + r) * CELL_SIZE;
                uint8_t color = block_colors[val];
                if (shadow) color = 13; // grey for shadow
                LCD_Draw_Rect(x, y, CELL_SIZE, CELL_SIZE, color, 1);
            }
        }
    }
}


// Draw shadow (landing preview) for current block
static void draw_shadow(void) {
    int shadow_row = current_block.row;
    while (can_place(current_block.type, current_block.rotation, shadow_row + 1, current_block.col)) {
        shadow_row++;
    }
    if (shadow_row != current_block.row) {
        draw_block(current_block.type, current_block.rotation, shadow_row, current_block.col, 1);
    }
}


// Draw next block preview (always at 0,0 in preview box)
static void draw_next_block(void) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            uint8_t val = tetriminos[next_block.type][next_block.rotation][r][c];
            if (val) {
                uint16_t x = NEXT_BLOCK_X + c * CELL_SIZE;
                uint16_t y = NEXT_BLOCK_Y + r * CELL_SIZE;
                LCD_Draw_Rect(x, y, CELL_SIZE, CELL_SIZE, block_colors[val], 1);
            }
        }
    }
}

// Definitions for helper functions (after tetrimino/grid declarations)
static int can_place(int type, int rotation, int row, int col) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            uint8_t val = tetriminos[type][rotation][r][c];
            if (!val) continue;
            int gr = row + r;
            int gc = col + c;
            if (gc < 0 || gc >= TETRIS_COLS) return 0;
            if (gr < 0 || gr >= TETRIS_ROWS) return 0;
            if (tetris_grid[gr][gc]) return 0;
        }
    }
    return 1;
}

static void lock_block(void) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            uint8_t val = tetriminos[current_block.type][current_block.rotation][r][c];
            if (val) {
                int gr = current_block.row + r;
                int gc = current_block.col + c;
                if (gr >= 0 && gr < TETRIS_ROWS && gc >= 0 && gc < TETRIS_COLS) {
                    tetris_grid[gr][gc] = val;
                }
            }
        }
    }
    // Clear full lines and count how many were cleared
    int lines_cleared = 0;
    for (int rr = TETRIS_ROWS - 1; rr >= 0; rr--) {
        int full = 1;
        for (int cc = 0; cc < TETRIS_COLS; cc++) {
            if (tetris_grid[rr][cc] == 0) { full = 0; break; }
        }
        if (full) {
            lines_cleared++;
            for (int move = rr; move > 0; move--) {
                for (int cc = 0; cc < TETRIS_COLS; cc++) {
                    tetris_grid[move][cc] = tetris_grid[move - 1][cc];
                }
            }
            for (int cc = 0; cc < TETRIS_COLS; cc++) tetris_grid[0][cc] = 0;
            rr++; // re-check this row after shifting
        }
    }

    // Play buzzer feedback: short beep for landing, pleasant tone for line clear
    if (lines_cleared > 0) {
        // Line clear melody (two short notes)
        buzzer_note(&buzzer_cfg, NOTE_E5, 45);
        HAL_Delay(100);
        buzzer_note(&buzzer_cfg, NOTE_G5, 45);
        HAL_Delay(140);
        buzzer_off(&buzzer_cfg);
        // Update score: award 100 points per line cleared
        Score_Add(lines_cleared * 100);
    } else {
        // Short landing beep
        buzzer_tone(&buzzer_cfg, 900, 35);
        HAL_Delay(40);
        buzzer_off(&buzzer_cfg);
    }
}

static void hard_drop(void) {
    while (can_place(current_block.type, current_block.rotation, current_block.row + 1, current_block.col)) {
        current_block.row++;
    }
    lock_block();
}

static void draw_tetris_grid(void) {
    // Draw grid background cells
    for (int r = 0; r < TETRIS_ROWS; r++) {
        for (int c = 0; c < TETRIS_COLS; c++) {
            uint8_t val = tetris_grid[r][c];
            uint16_t x = GRID_ORIGIN_X + c * CELL_SIZE;
            uint16_t y = GRID_ORIGIN_Y + r * CELL_SIZE;
            if (val) {
                LCD_Draw_Rect(x, y, CELL_SIZE, CELL_SIZE, block_colors[val], 1);
            } else {
                LCD_Draw_Rect(x, y, CELL_SIZE, CELL_SIZE, 0, 1);
            }
        }
    }

    // Outline playing field in purple (palette index 8)
    LCD_Draw_Rect(GRID_ORIGIN_X - 1, GRID_ORIGIN_Y - 1, GRID_WIDTH + 2, GRID_HEIGHT + 2, 8, 0);

    // Draw internal grid lines to mimic classic Tetris look (grey lines)
    for (int cc = 1; cc < TETRIS_COLS; cc++) {
        uint16_t x = GRID_ORIGIN_X + cc * CELL_SIZE;
        LCD_Draw_Line(x, GRID_ORIGIN_Y, x, GRID_ORIGIN_Y + GRID_HEIGHT - 1, 13);
    }
    for (int rr = 1; rr < TETRIS_ROWS; rr++) {
        uint16_t y = GRID_ORIGIN_Y + rr * CELL_SIZE;
        LCD_Draw_Line(GRID_ORIGIN_X, y, GRID_ORIGIN_X + GRID_WIDTH - 1, y, 13);
    }
}


MenuState Game1_Run(void) {
    // Initialize grid to empty
    for (int r = 0; r < TETRIS_ROWS; r++)
        for (int c = 0; c < TETRIS_COLS; c++)
            tetris_grid[r][c] = 0;

    // Seed RNG and select first/next block
    srand(HAL_GetTick());
    // Reset game-specific score tracker
    Score_Reset();
    current_block.type = random_block();
    current_block.rotation = 0;
    current_block.row = 0;
    current_block.col = (TETRIS_COLS - 4) / 2;
    next_block.type = random_block();
    next_block.rotation = 0;
    next_block.row = 0;
    next_block.col = (TETRIS_COLS - 4) / 2;

    // Play a brief startup sound
    buzzer_tone(&buzzer_cfg, 1000, 30);
    HAL_Delay(50);
    buzzer_off(&buzzer_cfg);

    MenuState exit_state = MENU_STATE_HOME;
    int game_over = 0;

    Joystick_cfg_t* joy_cfg = &joystick_cfg;
    Joystick_t* joy_data = &joystick_data;
    uint32_t last_fall = HAL_GetTick();
    int soft_drop = 0;
    int block_landed = 0;
    while (1) {
        uint32_t frame_start = HAL_GetTick();

        // Read input
        Input_Read();
        Joystick_Read(joy_cfg, joy_data);
        UserInput joy = Joystick_GetInput(joy_data);

        // Button handling: joystick push will be used for hard-drop during gameplay,
        // and for returning to menu only when game is over.

        // --- JOYSTICK CONTROLS ---
        uint32_t now = HAL_GetTick();
        // Left
        if ((joy.direction == W || joy.direction == NW || joy.direction == SW) && (now - last_left > JOY_REPEAT_DELAY)) {
            if (can_place(current_block.type, current_block.rotation, current_block.row, current_block.col - 1)) {
                current_block.col--;
                last_left = now;
            }
        } else if (joy.direction != W && joy.direction != NW && joy.direction != SW) {
            last_left = 0;
        }
        // Right
        if ((joy.direction == E || joy.direction == NE || joy.direction == SE) && (now - last_right > JOY_REPEAT_DELAY)) {
            if (can_place(current_block.type, current_block.rotation, current_block.row, current_block.col + 1)) {
                current_block.col++;
                last_right = now;
            }
        } else if (joy.direction != E && joy.direction != NE && joy.direction != SE) {
            last_right = 0;
        }
        // Rotate
        if (joy.direction == N && (now - last_up > JOY_REPEAT_DELAY)) {
            int new_rot = (current_block.rotation + 1) % 4;
            if (can_place(current_block.type, new_rot, current_block.row, current_block.col)) {
                current_block.rotation = new_rot;
                last_up = now;
            }
        } else if (joy.direction != N) {
            last_up = 0;
        }
        // Soft drop
        soft_drop = 0;
        if (joy.direction == S || joy.direction == SE || joy.direction == SW) {
            soft_drop = 1;
        }
        if (current_input.btn3_pressed) {
            if (!game_over) {
                // Instant hard drop during gameplay
                hard_drop();
                block_landed = 1;
            } else {
                // When game over, a button press should return to the menu
                exit_state = MENU_STATE_HOME;
                break;
            }
        }

        // --- BLOCK FALLING ---
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

        // --- RENDER ---
        LCD_Fill_Buffer(0);
        // Center the title horizontally
        const char *title = "TETRIS";
        size_t tlen = 0; while (title[tlen]) tlen++;
        const uint8_t title_font = 3;
        int title_x = (ST7789V2_WIDTH - (int)(tlen * 6 * title_font)) / 2;
        if (title_x < 0) title_x = 0;
        LCD_printString(title, title_x, 8, 1, title_font);
        draw_tetris_grid();
        draw_shadow();
        draw_block(current_block.type, current_block.rotation, current_block.row, current_block.col, 0);
        LCD_printString("Next:", NEXT_BLOCK_X, NEXT_BLOCK_Y - 18, 1, 2);
        draw_next_block();
        if (!game_over) {
            // Ensure control text is positioned safely above the bottom of the screen
            const uint8_t ctl_font = 1;
            const uint16_t font_h = 7 * ctl_font; // font5x7 is 7 pixels high
            const uint16_t margin = 6;
            uint16_t y1 = ST7789V2_HEIGHT - (font_h * 2) - margin; // first line
            uint16_t y2 = y1 + font_h + 3; // second line with small gap
            LCD_printString("Push joystick", 40, y1, 1, ctl_font);
            LCD_printString("to hard drop", 40, y2, 1, ctl_font);
        }
        // Draw left-hand score panel
        Score_Draw(&cfg0);
        LCD_Refresh(&cfg0);

        // --- BLOCK LANDED: next block ---
        if (block_landed) {
            current_block = next_block;
            current_block.row = 0;
            current_block.col = (TETRIS_COLS - 4) / 2;
            next_block.type = random_block();
            next_block.rotation = 0;
            block_landed = 0;
            // Game over check: if new block can't be placed, game over
            if (!can_place(current_block.type, current_block.rotation, current_block.row, current_block.col)) {
                // Show Game Over message
                game_over = 1;
                LCD_Fill_Buffer(0);
                // Center GAME OVER
                const char *go = "GAME OVER";
                size_t golen = 0; while (go[golen]) golen++;
                const uint8_t go_font = 3;
                int go_x = (ST7789V2_WIDTH - (int)(golen * 6 * go_font)) / 2;
                if (go_x < 0) go_x = 0;
                LCD_printString(go, go_x, 100, 2, go_font);
                // Show final score below
                char final_buf[32];
                snprintf(final_buf, sizeof(final_buf), "Final score: %d", Score_Get());
                size_t f_len = 0; while (final_buf[f_len]) f_len++;
                const uint8_t f_font = 2;
                int f_x = (ST7789V2_WIDTH - (int)(f_len * 6 * f_font)) / 2;
                if (f_x < 0) f_x = 0;
                LCD_printString(final_buf, f_x, 140, 1, f_font);
                LCD_Refresh(&cfg0);
                // Clear any existing button press so only a new press will exit
                Input_Read();
                while (current_input.btn3_pressed) {
                    Input_Read();
                    HAL_Delay(20);
                }

                // Play game-over music repeatedly until the user presses the joystick button.
                
                const Buzzer_Note_t melody[] = { NOTE_C5, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_F4, NOTE_E4 };
                const uint16_t m_dur[] = { 400, 300, 300, 300, 400, 400 };
                const int m_len = sizeof(melody) / sizeof(melody[0]);
                int stop = 0;
                while (!stop) {
                    for (int idx = 0; idx < m_len; idx++) {
                        // start note
                        buzzer_note(&buzzer_cfg, melody[idx], 45);
                        uint32_t note_start = HAL_GetTick();
                        while (HAL_GetTick() - note_start < m_dur[idx]) {
                            Input_Read();
                            if (current_input.btn3_pressed) {
                                stop = 1;
                                break;
                            }
                            HAL_Delay(20);
                        }
                        buzzer_off(&buzzer_cfg);
                        if (stop) break;
                    }
                    // brief pause between melody repeats
                    if (!stop) HAL_Delay(150);
                }

                // Wait for user to release the button (debounce) then register the press as exit
                while (current_input.btn3_pressed) {
                    Input_Read();
                    HAL_Delay(20);
                }
                // Now wait for the next press to exit
                while (1) {
                    Input_Read();
                    if (current_input.btn3_pressed) {
                        exit_state = MENU_STATE_HOME;
                        break;
                    }
                    HAL_Delay(20);
                }
                break;
            }
        }

        // Frame timing
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < GAME1_FRAME_TIME_MS) {
            HAL_Delay(GAME1_FRAME_TIME_MS - frame_time);
        }
    }
    return exit_state;
}
