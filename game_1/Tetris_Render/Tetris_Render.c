#include "Tetris_Render.h"
#include "Tetris_Config.h"
#include "Tetris_Pieces.h"
#include "Tetris_Logic.h"
#include "LCD.h"
#include "ST7789V2_Driver.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;

/* =========================================================
 *  Trail effect
 * ========================================================= */

typedef struct {
    int row, col, type, rotation;
    int valid;
} TrailEntry;

static TrailEntry trail[TRAIL_LEN];
static int        trail_head = 0;

/* Palette colours cycled for trail (fading from dim to invisible) */
/* indices lightest -> darkest dim ghost colours */
static const uint8_t trail_colors[TRAIL_LEN] = {13, 13, 0, 0};
/* 13 = grey, 0 = black (invisible) – older entries disappear */

void trail_reset(void) {
    for (int i = 0; i < TRAIL_LEN; i++) trail[i].valid = 0;
    trail_head = 0;
}

void trail_update(int row, int col, int type, int rotation) {
    /* Shift in new position */
    trail[trail_head].row      = row;
    trail[trail_head].col      = col;
    trail[trail_head].type     = type;
    trail[trail_head].rotation = rotation;
    trail[trail_head].valid    = 1;
    trail_head = (trail_head + 1) % TRAIL_LEN;

    /* Draw oldest → newest so newer overwrites older */
    for (int age = TRAIL_LEN - 1; age >= 0; age--) {
        int idx = (trail_head + age) % TRAIL_LEN;
        if (!trail[idx].valid) continue;
        /* Don't draw at same position as current block */
        if (trail[idx].row == row && trail[idx].col == col) continue;
        uint8_t col_idx = trail_colors[age]; /* age 0 = oldest */
        if (col_idx == 0) continue;
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                uint8_t val = tetriminos[trail[idx].type][trail[idx].rotation][r][c];
                if (!val) continue;
                int gr = trail[idx].row + r;
                int gc = trail[idx].col + c;
                if (gr < 0 || gr >= TETRIS_ROWS || gc < 0 || gc >= TETRIS_COLS) continue;
                /* Only draw in empty grid cells so we don't overwrite locked pieces */
                if (tetris_grid[gr][gc]) continue;
                uint16_t px = GRID_ORIGIN_X + gc * CELL_SIZE;
                uint16_t py = GRID_ORIGIN_Y + gr * CELL_SIZE;
                /* Draw a smaller inner rect to look like a faint trail */
                LCD_Draw_Rect(px + 2, py + 2, CELL_SIZE - 4, CELL_SIZE - 4, col_idx, 1);
            }
        }
    }
}

/* =========================================================
 *  Score popup
 * ========================================================= */

#define POPUP_TOTAL_FRAMES 40  /* ~1.2 s at 33 FPS */

static int    popup_value  = 0;
static int    popup_frames = 0;   /* frames remaining (0 = inactive) */
static int    popup_y_off  = 0;   /* pixel offset upward */

/* RGB cycling palette sequence for the popup text */
static const uint8_t popup_colors[] = {2, 5, 6, 3, 14, 15, 7}; /* R,O,Y,G,C,M,P */
#define POPUP_COLOR_COUNT 7

void score_popup_init(int delta) {
    popup_value  = delta;
    popup_frames = POPUP_TOTAL_FRAMES;
    popup_y_off  = 0;
}

int score_popup_active(void) {
    return popup_frames > 0;
}

void score_popup_update(void) {
    if (popup_frames <= 0) return;

    /* Move upward: 1 pixel every 2 frames */
    popup_y_off = (POPUP_TOTAL_FRAMES - popup_frames) / 2;

    /* Only draw while in the first half (fade by just stopping) */
    if (popup_frames > POPUP_TOTAL_FRAMES / 3) {
        char buf[8];
        snprintf(buf, sizeof(buf), "+%d", popup_value);
        /* Cycle colour based on remaining frames */
        uint8_t color_idx = popup_colors[(popup_frames / 4) % POPUP_COLOR_COUNT];
        /* Draw just above the grid centre, floating up */
        int base_y = GRID_ORIGIN_Y + GRID_HEIGHT / 2 - 10;
        int draw_y = base_y - (int)popup_y_off;
        if (draw_y < GRID_ORIGIN_Y) draw_y = GRID_ORIGIN_Y;
        /* Centre within the grid horizontally */
        int text_len = 0;
        while (buf[text_len]) text_len++;
        int draw_x = GRID_ORIGIN_X + (GRID_WIDTH - text_len * 6 * 2) / 2;
        if (draw_x < GRID_ORIGIN_X) draw_x = GRID_ORIGIN_X;
        LCD_printString(buf, (uint16_t)draw_x, (uint16_t)draw_y, color_idx, 2);
    }

    popup_frames--;
}

/* =========================================================
 *  Line-clear RGB flash
 * ========================================================= */

/* RGB cycling colours for the flash (use bright palette entries) */
static const uint8_t flash_palette[] = {2, 5, 6, 3, 14, 15, 7};
#define FLASH_PALETTE_LEN 7

void line_clear_flash(int lines_cleared) {
    if (lines_cleared <= 0) return;

    for (int frame = 0; frame < LINE_CLEAR_FLASH_FRAMES; frame++) {
        uint8_t color = flash_palette[frame % FLASH_PALETTE_LEN];
        /* Flash the entire cleared area (top rows of grid) */
        for (int row = 0; row < lines_cleared; row++) {
            uint16_t y = GRID_ORIGIN_Y + row * CELL_SIZE;
            LCD_Draw_Rect(GRID_ORIGIN_X, y, GRID_WIDTH, CELL_SIZE, color, 1);
        }
        /* Also flash the bottom rows (where clearing happens) – 
           we don't know exact rows post-clear, so flash full width stripes */
        LCD_Refresh(&cfg0);
        HAL_Delay(30);
    }
}

/* =========================================================
 *  Block rendering
 * ========================================================= */

void draw_block(int type, int rotation, int row, int col, int shadow) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            uint8_t val = tetriminos[type][rotation][r][c];
            if (val) {
                uint16_t x    = GRID_ORIGIN_X + (col + c) * CELL_SIZE;
                uint16_t y    = GRID_ORIGIN_Y + (row + r) * CELL_SIZE;
                uint8_t color = shadow ? 13 : block_colors[val];
                LCD_Draw_Rect(x, y, CELL_SIZE, CELL_SIZE, color, 1);
            }
        }
    }
}

void draw_shadow(void) {
    int shadow_row = current_block.row;
    while (can_place(current_block.type, current_block.rotation,
                     shadow_row + 1, current_block.col)) {
        shadow_row++;
    }
    if (shadow_row != current_block.row) {
        draw_block(current_block.type, current_block.rotation,
                   shadow_row, current_block.col, 1);
    }
}

void draw_next_block(void) {
    /* Draw NEXT_BLOCK_COUNT pieces stacked vertically in the preview panel */
    for (int n = 0; n < NEXT_BLOCK_COUNT; n++) {
        int type     = next_blocks[n].type;
        int rotation = next_blocks[n].rotation;
        uint16_t base_y = NEXT_BLOCK_Y + n * (4 * CELL_SIZE + NEXT_BLOCK_SPACING);
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                uint8_t val = tetriminos[type][rotation][r][c];
                if (val) {
                    uint16_t x = NEXT_BLOCK_X + c * CELL_SIZE;
                    uint16_t y = base_y + r * CELL_SIZE;
                    LCD_Draw_Rect(x, y, CELL_SIZE, CELL_SIZE, block_colors[val], 1);
                }
            }
        }
    }
}

void draw_tetris_grid(void) {
    /* Fill all cells */
    for (int r = 0; r < TETRIS_ROWS; r++) {
        for (int c = 0; c < TETRIS_COLS; c++) {
            uint8_t  val   = tetris_grid[r][c];
            uint16_t x     = GRID_ORIGIN_X + c * CELL_SIZE;
            uint16_t y     = GRID_ORIGIN_Y + r * CELL_SIZE;
            uint8_t  color = val ? block_colors[val] : 0;
            LCD_Draw_Rect(x, y, CELL_SIZE, CELL_SIZE, color, 1);
        }
    }

    /* Outline playing field in purple (palette index 8) */
    LCD_Draw_Rect(GRID_ORIGIN_X - 1, GRID_ORIGIN_Y - 1,
                  GRID_WIDTH + 2, GRID_HEIGHT + 2, 8, 0);

    /* Vertical grid lines (grey, palette index 13) */
    for (int cc = 1; cc < TETRIS_COLS; cc++) {
        uint16_t x = GRID_ORIGIN_X + cc * CELL_SIZE;
        LCD_Draw_Line(x, GRID_ORIGIN_Y, x, GRID_ORIGIN_Y + GRID_HEIGHT - 1, 13);
    }
    /* Horizontal grid lines */
    for (int rr = 1; rr < TETRIS_ROWS; rr++) {
        uint16_t y = GRID_ORIGIN_Y + rr * CELL_SIZE;
        LCD_Draw_Line(GRID_ORIGIN_X, y, GRID_ORIGIN_X + GRID_WIDTH - 1, y, 13);
    }
}

