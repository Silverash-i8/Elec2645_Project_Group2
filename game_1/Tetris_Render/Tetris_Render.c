#include "Tetris_Render.h"
#include "Tetris_Config.h"
#include "Tetris_Pieces.h"
#include "Tetris_Logic.h"
#include "LCD.h"

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
