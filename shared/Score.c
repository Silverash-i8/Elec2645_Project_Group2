#include "Score.h"
#include "LCD.h"
#include <stdio.h>

static int score_value = 0;

void Score_Init(void) {
    score_value = 0;
}

void Score_Reset(void) {
    score_value = 0;
}

void Score_Set(int value) {
    score_value = value;
}

void Score_Add(int delta) {
    score_value += delta;
    if (score_value < 0) score_value = 0;
}

int Score_Get(void) {
    return score_value;
}

// Draw a string scaled by a rational factor (num/den) using font5x7_ bitmap.
// Uses nearest-neighbour mapping from destination pixels to source pixels.
static void Score_printScaledString(const char *str, const uint16_t x0, const uint16_t y0, const uint8_t colour, const uint8_t num, const uint8_t den) {
    // scaled character advance (includes 1px spacing): round(6 * num / den)
    const uint16_t advance = (6 * num + den/2) / den;
    const uint16_t src_w = 5;
    const uint16_t src_h = 7;
    // scaled char pixel size
    const uint16_t dst_char_w = (src_w * num + den/2) / den;
    const uint16_t dst_char_h = (src_h * num + den/2) / den;

    uint16_t cx = x0;
    for (size_t ci = 0; str[ci]; ++ci) {
        unsigned char c = (unsigned char)str[ci];
        if (c < 32) c = 32;
        size_t idx = (c - 32) * 5;
        // for each destination pixel in char box
        for (uint16_t dx = 0; dx < dst_char_w; ++dx) {
            // map to source x
            uint16_t sx = (dx * den) / num;
            if (sx >= src_w) sx = src_w - 1;
            for (uint16_t dy = 0; dy < dst_char_h; ++dy) {
                uint16_t sy = (dy * den) / num;
                if (sy >= src_h) sy = src_h - 1;
                // check source pixel
                uint8_t src_byte = font5x7_[idx + sx];
                if (src_byte & (1u << sy)) {
                    // draw pixel scaled
                    LCD_Set_Pixel(cx + dx, y0 + dy, colour);
                }
            }
        }
        // advance to next character (including spacing)
        cx += advance;
    }
}

void Score_Draw(ST7789V2_cfg_t* cfg) {
    // Compact layout: draw both label and numeric value at 1.5x scale (3/2)
    char buf[32];
    const uint8_t num = 3;
    const uint8_t den = 2; // scale = 3/2 = 1.5
    const uint16_t label_y = 32; // below the title

    // Reserve a right boundary for the left-hand UI so it doesn't overlap the playfield.
    const uint16_t right_bound = 60;

    const char *label_text = "Score:";
    size_t label_len = 0;
    while (label_text[label_len]) label_len++;
    // scaled advance for label: round(6 * num / den)
    uint16_t label_advance = (6 * num + den/2) / den;
    uint16_t label_width = (uint16_t)(label_len * label_advance);

    uint16_t x = 4;
    if (label_width + 8 < right_bound) {
        x = right_bound - label_width - 4;
    }

    // Draw label and value scaled by 3/2
    Score_printScaledString(label_text, x, label_y, 1, num, den);

    snprintf(buf, sizeof(buf), "%d", score_value);
    size_t val_len = 0;
    while (buf[val_len]) val_len++;
    uint16_t val_advance = label_advance; // same advance since same scale
    uint16_t val_width = (uint16_t)(val_len * val_advance);
    uint16_t val_x = x + (label_width > val_width ? (label_width - val_width) / 2 : 0);
    uint16_t scaled_font_h = (7 * num + den/2) / den;
    uint16_t value_y = label_y + scaled_font_h + 4;
    Score_printScaledString(buf, val_x, value_y, 1, num, den);
}
