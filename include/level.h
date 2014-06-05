#pragma once

#include "utils.h"

#define TILE_DIM 32
#define LEVEL_MAX_DIM 20

typedef enum tile {
	VOID=0,
	STONEFLOOR,
	PLANKS,
	GRASS,
	SNOW,
	SAND,
	STONE=8,
	STONETOP,
	STONETORCH,
	SHRUB=16
} tile;

typedef struct level {
	tile tiles[LEVEL_MAX_DIM][LEVEL_MAX_DIM];
	char name[32];
	double ambience;
} level;

void initialize_level();
void reset_level();
void switch_level(char *name);
level *load_level(char *name);
void save_level(level *l);
level *get_current_level();
int is_solid_tile(int x, int y);
void draw_level();
void draw_level_top();
