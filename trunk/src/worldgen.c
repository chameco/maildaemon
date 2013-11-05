#include "worldgen.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <cuttle/debug.h>

void (*REGION_HANDLERS[256])(region *) = {NULL};

void save_world(world *w)
{
	FILE *f = fopen("world_save", "wb");
	fwrite(w, sizeof(int), 2, f);
	int x, y;
	for (x = 0; x < WORLD_DIM; ++x) {
		for (y = 0; y < WORLD_DIM; ++y) {
			save_region(f, w, x, y);
		}
	}
	fclose(f);
}

world *load_world()
{
	world *ret = (world *) malloc(sizeof(world));
	ret->player_x = 0;
	ret->player_y = 0;
	int x, y;
	for (x = 0; x < WORLD_DIM; ++x) {
		for (y = 0; y < WORLD_DIM; ++y) {
			ret->regions[x][y] = NULL;
		}
	}
	load_region(ret, ret->player_x, ret->player_y);
	return ret;
}

void unload_world(world *w)
{
	int x, y;
	for (x = 0; x < WORLD_DIM; ++x) {
		for (y = 0; y < WORLD_DIM; ++y) {
			free(w->regions[x][y]);
		}
	}
	free(w);

}

int get_current_x_coord(world *w)
{
	return w->player_x;
}

int get_current_y_coord(world *w)
{
	return w->player_y;
}

region *get_region(world *w, int x, int y)
{
	load_region(w, x, y);
	return w->regions[x][y];
}

region *read_region(FILE *f, int x, int y)
{
	long offset = sizeof(world) - sizeof(region *[WORLD_DIM]);
	fseek(f, offset + (sizeof(region) * WORLD_DIM * y) + (sizeof(region) * x), SEEK_SET);
	region *r = (region *) malloc(sizeof(region));
	fread(r, sizeof(region), 1, f);
	return r;
}

void save_region(FILE *f, world *w, int x, int y)
{
	long offset = sizeof(world) - sizeof(region *[WORLD_DIM]);
	fseek(f, offset + (sizeof(region) * WORLD_DIM * y) + (sizeof(region) * x), SEEK_SET);
	fwrite(w->regions[x][y], sizeof(region), 1, f);
}

void load_region(world *w, int x, int y)
{
	if (w->regions[x][y] == NULL) {
		FILE *f = fopen("world_save", "rb");
		w->regions[x][y] = read_region(f, x, y);
		fclose(f);
	}
	w->player_x = x;
	w->player_y = y;
}

void unload_region(world *w, int x, int y) {
	
	if (w->regions[x][y] != NULL) {
		FILE *f = fopen("world_save", "rb");
		save_region(f, w, x, y);
		fclose(f);
		free(w->regions[x][y]);
		w->regions[x][y] = NULL;
	}
}

region *generate_region(char *name, int width, int height, int temperature, double altitude)
{
	region *ret = (region *) malloc(sizeof(region));
	strcpy(ret->name, name);
	ret->width = width;
	ret->height = height;
	ret->temperature = temperature;
	ret->altitude = altitude;
	ret->humidity = 0.5;
	if (temperature < 10) {
		if (altitude < 0.25) {
			ret->terrain_type = ICEVLLY;
		} else if (altitude < 0.5) {
			ret->terrain_type = TUNDRA;
		} else if (altitude < 0.75) {
			ret->terrain_type = TAIGA;
		} else {
			ret->terrain_type = ICEMTNS;
		}
	} else if (temperature < 70) {
		if (altitude < 0.25) {
			ret->terrain_type = VALLEY;
		} else if (altitude < 0.5) {
			ret->terrain_type = PLAIN;
		} else if (altitude < 0.75) {
			ret->terrain_type = FOREST;
		} else {
			ret->terrain_type = MOUNTAINS;
		}
	} else {
		if (altitude < 0.25) {
			ret->terrain_type = CRATER;
		} else if (altitude < 0.5) {
			ret->terrain_type = DESERT;
		} else if (altitude < 0.75) {
			ret->terrain_type = OASIS;
		} else {
			ret->terrain_type = DUNES;
		}
	}
	if (REGION_HANDLERS[ret->terrain_type] != NULL) {
		REGION_HANDLERS[ret->terrain_type](ret);
	} else {
		log_err("Invalid terrain type for region %s", name);
		exit(1);
	}
	return ret;
}

void register_region_handler(int terrain_type, void (*handler)(region *))
{
	REGION_HANDLERS[terrain_type] = handler;
}

void register_standard_handlers()
{
	register_region_handler(ICEVLLY, handler_icevlly);
	register_region_handler(TUNDRA, handler_tundra);
	register_region_handler(TAIGA, handler_taiga);
	register_region_handler(ICEMTNS, handler_icemtns);

	register_region_handler(VALLEY, handler_valley);
	register_region_handler(PLAIN, handler_plain);
	register_region_handler(FOREST, handler_forest);
	register_region_handler(MOUNTAINS, handler_mountains);

	register_region_handler(CRATER, handler_crater);
	register_region_handler(DESERT, handler_desert);
	register_region_handler(OASIS, handler_oasis);
	register_region_handler(DUNES, handler_dunes);
}

void handler_icevlly(region *r)
{
	r->ambience = 0.0;
	r->humidity = 0.1;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			if (rand() % 4 == 0) {
				r->blocks[x][y] = SAND;
			} else {
				r->blocks[x][y] = SNOW;
			}
		}
	}
}

void handler_tundra(region *r)
{
	r->ambience = 0.0;
	r->humidity = 0.1;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			r->blocks[x][y] = SNOW;
		}
	}
}

void handler_taiga(region *r)
{
	r->ambience = 0.2;
	r->humidity = 0.2;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			if (rand() % 4 == 0) {
				r->blocks[x][y] = SHRUB;
			} else {
				r->blocks[x][y] = SNOW;
			}
		}
	}
}

void handler_icemtns(region *r)
{
	r->ambience = 0.0;
	r->humidity = 0.1;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			if (rand() % 4 == 0) {
				r->blocks[x][y] = STONE;
			} else {
				r->blocks[x][y] = SNOW;
			}
		}
	}
}

void handler_valley(region *r)
{
	r->ambience = 0.2;
	r->humidity = 0.5;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			if (rand() % 4 == 0) {
				r->blocks[x][y] = SAND;
			} else {
				r->blocks[x][y] = GRASS;
			}
		}
	}
}

void handler_plain(region *r)
{
	r->ambience = 0.2;
	r->humidity = 0.5;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			r->blocks[x][y] = GRASS;
		}
	}
}

void handler_forest(region *r)
{
	r->ambience = 0.5;
	r->humidity = 0.7;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			if (rand() % 4 == 0) {
				r->blocks[x][y] = SHRUB;
			} else {
				r->blocks[x][y] = GRASS;
			}
		}
	}
}

void handler_mountains(region *r)
{
	r->ambience = 0.2;
	r->humidity = 0.5;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			if (rand() % 4 == 0) {
				r->blocks[x][y] = STONE;
			} else {
				r->blocks[x][y] = PLANKS;
			}
		}
	}
}

void handler_crater(region *r)
{
	r->ambience = 0.0;
	r->humidity = 0.0;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			if (rand() % 4 == 0) {
				r->blocks[x][y] = PLANKS;
			} else {
				r->blocks[x][y] = SAND;
			}
		}
	}
}

void handler_desert(region *r)
{
	r->ambience = 0.0;
	r->humidity = 0.0;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			if (rand() % 8 == 0) {
				r->blocks[x][y] = STONE;
			} else {
				r->blocks[x][y] = SAND;
			}
		}
	}
}

void handler_oasis(region *r)
{
	r->ambience = 0.2;
	r->humidity = 0.7;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			if (rand() % 4 == 0) {
				r->blocks[x][y] = SHRUB;
			} else {
				r->blocks[x][y] = SAND;
			}
		}
	}
}

void handler_dunes(region *r)
{
	r->ambience = 0.0;
	r->humidity = 0.0;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			int n = rand() % 4;
			if (n == 0) {
				r->blocks[x][y] = STONE;
			} else if (n == 1) {
				r->blocks[x][y] = PLANKS;
			} else {
				r->blocks[x][y] = SAND;
			}
		}
	}
}

int is_solid_block(region *r, int x, int y)
{
	if (x >= 0 && x < r->width) {
		if (y >= 0 && y < r->height) {
			if (r->blocks[x][y] <= PLANKS) {
				return 0;
			}
		}
	}
	return 1;
}

void set_current_region(world *w, int x, int y)
{
	load_region(w, x, y);
	w->player_x = x;
	w->player_y = y;
}

region *get_current_region(world *w)
{
	return get_region(w, w->player_x, w->player_y);
}

char *get_current_region_name(world *w)
{
	return get_current_region(w)->name;
}

double get_current_region_ambience(world *w)
{
	return get_current_region(w)->ambience;
}
