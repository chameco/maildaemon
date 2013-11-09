#ifndef PURGE_WORLDGEN_H
#define PURGE_WORLDGEN_H
#include <stdio.h>

#define WORLD_DIM 100

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
	block blocks[10][10];
} region;

typedef struct world {
	int player_x, player_y;
	region *regions[WORLD_DIM][WORLD_DIM];
} world;

void save_world(world *w);
world *load_world();
void unload_world(world *w);

region *get_region(world *w, int x, int y);
region *read_region(FILE *f, int x, int y);
void save_region(FILE *f, world *w, int x, int y);
void load_region(world *w, int x, int y);
void unload_region(world *w, int x, int y);

region *generate_region(char *name, int width, int height);
void add_region_handler(void (*handler)(region *));
void register_standard_handlers();

void handler_emptyroom(region *r);

int is_solid_block(region *r, int x, int y);
void set_current_region(world *w, int x, int y);
region *get_current_region(world *w);

#endif
