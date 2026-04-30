#ifndef TETRIS_CONFIG_H
#define TETRIS_CONFIG_H

/* Joystick debounce timing (milliseconds) */
#define JOY_REPEAT_DELAY 150
#define JOY_FAST_REPEAT   60

/* Grid geometry */
#define TETRIS_ROWS   16
#define TETRIS_COLS   10
#define CELL_SIZE     10
#define GRID_WIDTH    (TETRIS_COLS * CELL_SIZE)
#define GRID_HEIGHT   (TETRIS_ROWS * CELL_SIZE)
#define GRID_ORIGIN_X 65   /* Centred horizontally for 240x240 LCD */
#define GRID_ORIGIN_Y 36   /* Lowered to avoid title overlap        */
#define NEXT_BLOCK_X  (GRID_ORIGIN_X + GRID_WIDTH + 18)
#define NEXT_BLOCK_Y  (GRID_ORIGIN_Y + 20)

/* Frame rate (~33 FPS) */
#define GAME1_FRAME_TIME_MS 30

#endif /* TETRIS_CONFIG_H */
