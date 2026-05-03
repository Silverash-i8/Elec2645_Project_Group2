#include "../Map/Map.h"
#include "LCD.h"
#include "../Sprites/brick_sprite.h"
#include "../Sprites/steel_sprite.h"
#include "../Sprites/Base eagle.h"

static TileType gameMap[MAP_HEIGHT][MAP_WIDTH];

// All maps are hardcoded for simplicity

void Map_Init_Easy() {
    // Initialize the map with empty tiles
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            gameMap[row][col] = TILE_EMPTY;
        }
    }

    // Fewer bricks for easy difficulty
    // Brick Group 1 - L-shape in top-left (smaller)
    for(int row = 6; row < 8; row++){
        gameMap[row][3] = TILE_BRICK;
    }
    for(int col = 3; col < 5; col++){
        gameMap[8][col] = TILE_BRICK;
    }
    
    // Brick Group 2 - L-shape in top-right (smaller)
    for(int row = 6; row < 8; row++){
        gameMap[row][20] = TILE_BRICK;
    }
    for(int col = 19; col < 21; col++){
        gameMap[8][col] = TILE_BRICK;
    }
    
    // Steel obstacles - small 2x2 blocks for added difficulty
    gameMap[10][10] = TILE_STEEL;
    gameMap[10][11] = TILE_STEEL;
    gameMap[11][10] = TILE_STEEL;
    gameMap[11][11] = TILE_STEEL;
    
    gameMap[10][12] = TILE_STEEL;
    gameMap[10][13] = TILE_STEEL;
    gameMap[11][12] = TILE_STEEL;
    gameMap[11][13] = TILE_STEEL;
    
    // Brick wall around the base for protection
    // Top wall
    for(int col = 10; col < 15; col++){
        gameMap[19][col] = TILE_BRICK;
    }
    // Left wall
    gameMap[20][10] = TILE_BRICK;
    gameMap[21][10] = TILE_BRICK;
    // Right wall
    gameMap[20][14] = TILE_BRICK;
    gameMap[21][14] = TILE_BRICK;

    // Place the base in the center bottom
    gameMap[MAP_HEIGHT - 1][MAP_WIDTH / 2] = TILE_BASE;
}

void Map_Init_Medium() {
    // Initialize the map with empty tiles
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            gameMap[row][col] = TILE_EMPTY;
        }
    }
    
    // Medium difficulty - original layout
    // Brick Group 1 - L-shape in top-left
    for(int row = 5; row < 8; row++){
        gameMap[row][2] = TILE_BRICK;
    }
    for(int col = 2; col < 5; col++){
        gameMap[7][col] = TILE_BRICK;
    }
    
    // Brick Group 2 - L-shape in top-right (mirrored)
    for(int row = 5; row < 8; row++){
        gameMap[row][21] = TILE_BRICK;
    }
    for(int col = 19; col < 22; col++){
        gameMap[7][col] = TILE_BRICK;
    }
    
    // Brick Group 3 - Center horizontal line
    for(int col = 9; col < 15; col++){
        gameMap[10][col] = TILE_BRICK;
    }
    
    // Brick Group 4 - Left center square (4-block pattern)
    for(int row = 13; row < 15; row++){
        for(int col = 3; col < 5; col++){
            gameMap[row][col] = TILE_BRICK;
        }
    }
    
    // Brick Group 5 - Right center square (4-block pattern)
    for(int row = 13; row < 15; row++){
        for(int col = 19; col < 21; col++){
            gameMap[row][col] = TILE_BRICK;
        }
    }
    
    // Brick wall around the base for protection
    // Top wall
    for(int col = 10; col < 15; col++){
        gameMap[19][col] = TILE_BRICK;
    }
    // Left wall
    gameMap[20][10] = TILE_BRICK;
    gameMap[21][10] = TILE_BRICK;
    // Right wall
    gameMap[20][14] = TILE_BRICK;
    gameMap[21][14] = TILE_BRICK;
    
    // Steel Group 1 - Left protected area
    gameMap[12][10] = TILE_STEEL;
    gameMap[12][11] = TILE_STEEL;
    gameMap[13][10] = TILE_STEEL;
    gameMap[13][11] = TILE_STEEL;
    
    // Steel Group 2 - Right protected area
    gameMap[12][12] = TILE_STEEL;
    gameMap[12][13] = TILE_STEEL;
    gameMap[13][12] = TILE_STEEL;
    gameMap[13][13] = TILE_STEEL;
    
    // Place the base in the center bottom
    gameMap[MAP_HEIGHT - 1][MAP_WIDTH / 2] = TILE_BASE;
}

void Map_Init_Hard() {
    // Initialize the map with empty tiles
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            gameMap[row][col] = TILE_EMPTY;
        }
    }
    
    // Hard difficulty - dense brick maze
    // Brick Group 1 - Large L-shape in top-left
    for(int row = 4; row < 9; row++){
        gameMap[row][2] = TILE_BRICK;
    }
    for(int col = 2; col < 6; col++){
        gameMap[8][col] = TILE_BRICK;
    }
    
    // Brick Group 2 - Large L-shape in top-right
    for(int row = 4; row < 9; row++){
        gameMap[row][21] = TILE_BRICK;
    }
    for(int col = 18; col < 22; col++){
        gameMap[8][col] = TILE_BRICK;
    }
    
    // Brick Group 3 - Two horizontal lines in center
    for(int col = 8; col < 16; col++){
        gameMap[9][col] = TILE_BRICK;
    }
    for(int col = 8; col < 16; col++){
        gameMap[12][col] = TILE_BRICK;
    }
    
    // Brick Group 4 - Left center double square
    for(int row = 13; row < 16; row++){
        for(int col = 2; col < 5; col++){
            gameMap[row][col] = TILE_BRICK;
        }
    }
    
    // Brick Group 5 - Right center double square
    for(int row = 13; row < 16; row++){
        for(int col = 19; col < 22; col++){
            gameMap[row][col] = TILE_BRICK;
        }
    }
    
    
    // Brick wall around the base for protection
    // Top wall
    for(int col = 10; col < 15; col++){
        gameMap[19][col] = TILE_BRICK;
    }
    // Left wall
    gameMap[20][10] = TILE_BRICK;
    gameMap[21][10] = TILE_BRICK;
    // Right wall
    gameMap[20][14] = TILE_BRICK;
    gameMap[21][14] = TILE_BRICK;
    
    // Steel Group 1 & 2 - Extended protection
    for(int row = 11; row < 14; row++){
        gameMap[row][10] = TILE_STEEL;
        gameMap[row][13] = TILE_STEEL;
    }
    
    // Place the base in the center bottom
    gameMap[MAP_HEIGHT - 1][MAP_WIDTH / 2] = TILE_BASE;
}

void Map_Draw(uint8_t base_health) {
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            uint16_t x = col * TILE_SIZE;
            uint16_t y = row * TILE_SIZE + MAP_OFFSET_Y; // Applying offset to Y coordinate
            switch (gameMap[row][col]) {
                case TILE_EMPTY:
                    // Draw empty tile (black)
                    LCD_Draw_Rect(x, y, TILE_SIZE, TILE_SIZE, 0, 1);
                    break;
                case TILE_BRICK:
                    // Draw brick sprite 
                    LCD_Draw_Sprite(x, y, BRICK_FRAME_WIDTH, BRICK_FRAME_HEIGHT, (const uint8_t *)brick_data);
                    break;
                case TILE_STEEL:
                    // Draw steel sprite 
                    LCD_Draw_Sprite(x, y, STEEL_FRAME_WIDTH, STEEL_FRAME_HEIGHT, (const uint8_t *)steel_data);
                    break;
                case TILE_BASE:
                    // Draw base sprite only if it hasn't been destroyed
                    if (base_health > 0) {
                        LCD_Draw_Sprite(x, y, BASE_EAGLE_FRAME_WIDTH, BASE_EAGLE_FRAME_HEIGHT, (const uint8_t *)base_eagle_data);
                    } else {
                        // Draw black tile where base was destroyed
                        LCD_Draw_Rect(x, y, TILE_SIZE, TILE_SIZE, 0, 1);
                    }
                    break;
            }
        }
    }
    // Map Boundary
    LCD_Draw_Rect(0,MAP_OFFSET_Y,MAP_WIDTH * TILE_SIZE,MAP_HEIGHT * TILE_SIZE,4,0);
}
uint8_t Map_IsSolid(uint8_t row, uint8_t col) {
    if (row >= MAP_HEIGHT || col >= MAP_WIDTH) {
        return 1; // Out of bounds is considered solid
    }
    if (gameMap[row][col] == TILE_EMPTY)
        return 0;
    return 1; // Any non-empty tile is solid
}
void Map_DestroyTile(uint8_t row, uint8_t col) {
        if (gameMap[row][col] == TILE_BRICK || gameMap[row][col] == TILE_BASE) {
            gameMap[row][col] = TILE_EMPTY; // Destroy Brick or Base
        }
        // Steel cannot be destroyed
}