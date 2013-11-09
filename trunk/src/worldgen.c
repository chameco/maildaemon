#include "worldgen.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <cuttle/debug.h>

void (*REGION_HANDLERS[256])(region *) = {NULL};
int REGION_HANDLERS_INDEX = 0;

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

region *generate_region(char *name, int width, int height)
{
	region *ret = (region *) malloc(sizeof(region));
	strcpy(ret->name, name);
	ret->width = width;
	ret->height = height;
	REGION_HANDLERS[rand() % REGION_HANDLERS_INDEX](ret);
	return ret;
}

void add_region_handler(void (*handler)(region *))
{
	REGION_HANDLERS[REGION_HANDLERS_INDEX++] = handler;
}

void register_standard_handlers()
{
	add_region_handler(handler_emptyroom);
}

void handler_emptyroom(region *r)
{
	r->ambience = 0.8;
	int x, y;
	for (x = 0; x < r->width; ++x) {
		for (y = 0; y < r->height; ++y) {
			if (x == 4 && y == 4) {
				r->blocks[x][y] = TORCH;
			} else if (x == 0 || y == 0 || x == r->width-1 || y == r->height-1) {
				if ((x == (r->width / 2) - 1 || x == (r->width / 2))
						|| (y == (r->height / 2) - 1 || y == (r->height / 2))) {
					r->blocks[x][y] = PLANKS;
				} else {
					r->blocks[x][y] = STONE;
				}
			} else {
				r->blocks[x][y] = PLANKS;
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
