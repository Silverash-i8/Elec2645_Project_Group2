#ifndef TETRIS_RENDER_H
#define TETRIS_RENDER_H

/* Draw a piece at the given grid position.
   Pass shadow = 1 to render as a ghost/shadow piece. */
void draw_block(int type, int rotation, int row, int col, int shadow);

/* Draw the drop-shadow preview for current_block */
void draw_shadow(void);

/* Draw the next-piece preview box */
void draw_next_block(void);

/* Draw the full playing field (background cells + border + grid lines) */
void draw_tetris_grid(void);

#endif /* TETRIS_RENDER_H */
