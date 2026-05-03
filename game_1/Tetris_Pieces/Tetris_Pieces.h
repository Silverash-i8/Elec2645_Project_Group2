#ifndef TETRIS_PIECES_H
#define TETRIS_PIECES_H

#include <stdint.h>

/* State of a single tetrimino on the board */
typedef struct {
    int type;      /* Piece index 0-6                          */
    int rotation;  /* Rotation state 0-3                       */
    int row, col;  /* Top-left corner of 4x4 bounding box      */
} BlockState;

/* Maps block value (1-7) to LCD palette index. Index 0 = empty cell. */
extern const uint8_t block_colors[8];

/* Shape data layout: [piece][rotation][row][col] */
extern const uint8_t tetriminos[7][4][4][4];

/* Returns a random piece type in range [0, 6] */
int random_block(void);

#endif /* TETRIS_PIECES_H */
