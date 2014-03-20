#ifndef PURGE_WORLDGEN_H
#define PURGE_WORLDGEN_H
#include <stdio.h>
#include "entity.h"
#include "lights.h"
#include "fx.h"

#define WORLD_DIM 10

typedef enum room_type {
	EMPTY,
	HALL_HORIZ,
	HALL_VERT
} room_type;

typedef enum block {
	VOID,
	SNOW,
	GRASS,
	SAND,
	PLANKS,
	STONE,
	SHRUB,
	SNOWBANK,
	SANDSTONE,
	TORCH
} block;

typedef struct region {
	char name[256];
	int width, height;
	double ambience;
	block blocks[20][20];
	int numentities;
	char entities[10][512];
	int numlights;
	char lights[10][512];
	int numfx;
	char fx[10][512];
} region;

typedef struct world {
	int player_x, player_y;
	int player_position_x, player_position_y;
	region *regions[WORLD_DIM][WORLD_DIM];
} world;

void save_world(world *w);
world *load_world();
void unload_world(world *w);

region *get_region(world *w, int x, int y);
region *read_region(FILE *f, int x, int y);
void save_region(FILE *f, world *w, int x, int y);
void load_region(world *w, int x, int y);
void load_region_assets(region *r);
void unload_region(world *w, int x, int y);

region *generate_region(char *name, int width, int height);
void populate_region(region *r, int rtype);
void add_standard_room(room_type rt, region *r);

int is_solid_block(region *r, int x, int y);
void set_current_region(world *w, int x, int y);
region *get_current_region(world *w);

#endif
