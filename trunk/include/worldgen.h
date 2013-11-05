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
	enum {
		ICEVLLY, TUNDRA, TAIGA, ICEMTNS,
		VALLEY, PLAIN, FOREST, MOUNTAINS,
		CRATER, DESERT, OASIS, DUNES,
	} terrain_type;
	int temperature;
	double altitude;
	double humidity;
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

int get_current_x_coord(world *w);
int get_current_y_coord(world *w);
region *get_region(world *w, int x, int y);
region *read_region(FILE *f, int x, int y);
void save_region(FILE *f, world *w, int x, int y);
void load_region(world *w, int x, int y);
void unload_region(world *w, int x, int y);

region *generate_region(char *name, int width, int height, int temperature, double altitude);
void register_region_handler(int terrain_type, void (*handler)(region *));
void register_standard_handlers();

void handler_icevlly(region *r);
void handler_tundra(region *r);
void handler_taiga(region *r);
void handler_icemtns(region *r);

void handler_valley(region *r);
void handler_plain(region *r);
void handler_forest(region *r);
void handler_mountains(region *r);

void handler_crater(region *r);
void handler_desert(region *r);
void handler_oasis(region *r);
void handler_dunes(region *r);

int is_solid_block(region *r, int x, int y);
void set_current_region(world *w, int x, int y);
region *get_current_region(world *w);
char *get_current_region_name(world *w);
double get_current_region_ambience(world *w);

#endif
