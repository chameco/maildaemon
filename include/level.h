#pragma once

#include <stdbool.h>

#include "utils.h"

#define TILE_DIM 32
#define LEVEL_MAX_DIM 40

typedef enum tile {
	VOID=0,
	STONEFLOOR,
	PLANKS,
	GRASS,
	SNOW,
	SAND,
	CARPET,
	STONE=8,
	STONETOP,
	STONETORCH,
	STATUE,
	SHRUB=16,
	WATER
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
double get_current_spawn_x();
double get_current_spawn_y();
bool is_solid_tile(int x, int y);
void draw_level();
void draw_level_top();
