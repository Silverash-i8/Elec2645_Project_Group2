/**
 * @file Map.h
 * @brief Map for the Tank 1990 game
 * 
 * Controls all the map layout & environment.
 */

#ifndef MAP_H
#define MAP_H

#include <stdint.h>

#define MAP_WIDTH 24
#define MAP_HEIGHT 22
#define TILE_SIZE 10 
#define MAP_OFFSET_Y 20  // shift map down
typedef enum{
    TILE_EMPTY = 0,
    TILE_BRICK,
    TILE_STEEL,
    TILE_BASE
} TileType;

/**
 * @brief Initialize the map for easy difficulty
 */
void Map_Init_Easy();
/**
 * @brief Initialize the map for medium difficulty
 */
void Map_Init_Medium();
/**
 * @brief Initialize the map for hard difficulty
 */
void Map_Init_Hard();
/**
 * @brief Draw the map with the specified base health
 * @param base_health; Current health of the base
 */
void Map_Draw(uint8_t base_health);
/**
 * @brief Check if a tile is solid (impassable)
 * @param row Row of the tile
 * @param col Column of the tile
 * @brief Used in Tank_Update in Tank.c to check for collisions with walls
 * @return 1 if solid, 0 otherwise
 */
uint8_t Map_IsSolid(uint8_t row, uint8_t col);
/**
 * @brief Destroy a tile at the specified location
 * @param row Row of the tile to destroy
 * @param col Column of the tile to destroy
 * @brief Used in Bullet_Update in Bullet.c when a bullet hits a brick or the base
 */
void Map_DestroyTile(uint8_t row, uint8_t col);
#endif