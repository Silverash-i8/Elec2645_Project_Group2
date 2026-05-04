#ifndef TETRIS_RENDER_H
#define TETRIS_RENDER_H

#include <stdint.h>
#include "Tetris_Config.h"

/* Draw a piece at the given grid position.
   Pass shadow = 1 to render as a ghost/shadow piece. */
void draw_block(int type, int rotation, int row, int col, int shadow);

/* Draw the drop-shadow preview for current_block */
void draw_shadow(void);

/* Draw all NEXT_BLOCK_COUNT upcoming pieces in the preview panel */
void draw_next_block(void);

/* Draw the full playing field (background cells + border + grid lines) */
void draw_tetris_grid(void);

/* Trail state: call once per frame to update and render the trail behind
   the falling block.  Pass current row each frame. */
void trail_update(int row, int col, int type, int rotation);
void trail_reset(void);

/* RGB line-clear flash: call after a line is cleared with the number of
   lines cleared.  Drives the flash animation for LINE_CLEAR_FLASH_FRAMES. */
void line_clear_flash(int lines_cleared);

/* Floating score popup: initialise with score delta when lines are cleared */
void score_popup_init(int delta);

/* Update and draw the floating score popup (call each frame) */
void score_popup_update(void);

/* Returns 1 while the popup is still visible */
int score_popup_active(void);

#endif /* TETRIS_RENDER_H */
