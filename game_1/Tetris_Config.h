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
#define GRID_ORIGIN_X 75   /* Grid centre ~x125, balanced between score panel and right edge */
#define GRID_ORIGIN_Y 52   /* Lowered to give more breathing room under the title */
#define NEXT_BLOCK_X  (GRID_ORIGIN_X + GRID_WIDTH + 8)
#define NEXT_BLOCK_Y  (GRID_ORIGIN_Y + 16)

/* Number of upcoming blocks shown in the preview panel */
#define NEXT_BLOCK_COUNT 3

/* Vertical spacing between preview blocks (pixels) */
#define NEXT_BLOCK_SPACING 6

/* Frame rate (~60 FPS) */
#define GAME1_FRAME_TIME_MS 16

/* Trail effect: how many past positions to remember */
#define TRAIL_LEN 4

/* Line-clear flash: number of flash frames */
#define LINE_CLEAR_FLASH_FRAMES 6

#endif /* TETRIS_CONFIG_H */
