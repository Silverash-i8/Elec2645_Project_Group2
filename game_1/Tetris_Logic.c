#include "Tetris_Logic.h"
#include "Score.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"

extern Buzzer_cfg_t buzzer_cfg;

/* ---- Game state ---- */
uint8_t    tetris_grid[TETRIS_ROWS][TETRIS_COLS];
BlockState current_block;
BlockState next_block;

/* ---- Logic functions ---- */

int can_place(int type, int rotation, int row, int col) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            uint8_t val = tetriminos[type][rotation][r][c];
            if (!val) continue;
            int gr = row + r;
            int gc = col + c;
            if (gc < 0 || gc >= TETRIS_COLS) return 0;
            if (gr < 0 || gr >= TETRIS_ROWS) return 0;
            if (tetris_grid[gr][gc])          return 0;
        }
    }
    return 1;
}

void lock_block(void) {
    /* Stamp current block into the grid */
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

    /* Clear full lines */
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
            rr++; /* re-check this row after shifting */
        }
    }

    /* Audio + score feedback */
    if (lines_cleared > 0) {
        buzzer_note(&buzzer_cfg, NOTE_E5, 45);
        HAL_Delay(100);
        buzzer_note(&buzzer_cfg, NOTE_G5, 45);
        HAL_Delay(140);
        buzzer_off(&buzzer_cfg);
        Score_Add(lines_cleared * 100);
    } else {
        buzzer_tone(&buzzer_cfg, 900, 35);
        HAL_Delay(40);
        buzzer_off(&buzzer_cfg);
    }
}

void hard_drop(void) {
    while (can_place(current_block.type, current_block.rotation,
                     current_block.row + 1, current_block.col)) {
        current_block.row++;
    }
    lock_block();
}
