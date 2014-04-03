#ifndef PURGE_LEVEL_H
#define PURGE_LEVEL_H
#include "utils.h"

#define TILE_DIM 32
#define LEVEL_MAX_DIM 20

typedef enum tile {
	PLANKS,
	VOID,
	SNOW,
	GRASS,
	SAND,
	STONE,
	SHRUB,
	TORCH
} tile;

typedef struct level {
	tile tiles[LEVEL_MAX_DIM][LEVEL_MAX_DIM];
	char name[32];
	double ambience;
} level;

void initialize_level();
void switch_level(char *path);
level *load_level(char *path);
void save_level(level *l, char *path);
level *get_current_level();
int is_solid_tile(int x, int y);
void draw_level();
#endif
