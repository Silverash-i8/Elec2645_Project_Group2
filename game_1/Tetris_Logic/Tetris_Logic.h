#ifndef TETRIS_LOGIC_H
#define TETRIS_LOGIC_H

#include <stdint.h>
#include "Tetris_Config.h"
#include "Tetris_Pieces.h"

/* Shared game state – defined in Tetris_Logic.c */
extern uint8_t    tetris_grid[TETRIS_ROWS][TETRIS_COLS];
extern BlockState current_block;
extern BlockState next_block;

/* Returns 1 if the piece can be placed at (row, col), 0 otherwise */
int  can_place(int type, int rotation, int row, int col);

/* Locks current_block into the grid, clears full lines, updates score */
void lock_block(void);

/* Drops current_block to its lowest valid row then calls lock_block() */
void hard_drop(void);

#endif /* TETRIS_LOGIC_H */
