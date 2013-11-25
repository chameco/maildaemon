#include "worldgen.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <cuttle/debug.h>

#include "player.h"
#include "entity.h"
#include "lights.h"
#include "fx.h" 
region *STANDARD_ROOMS[32];
int STANDARD_ROOMS_INDEX = 0;

void save_world(world *w)
{
	FILE *f = fopen("world_save", "wb");
	w->player_position_x = get_player_x();
	w->player_position_y = get_player_y();
	fwrite(w, sizeof(int), 4, f);
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
	FILE *f = fopen("world_save", "rb");
	fread(&(ret->player_x), sizeof(int), 1, f);
	fread(&(ret->player_y), sizeof(int), 1, f);
	fread(&(ret->player_position_x), sizeof(int), 1, f);
	fread(&(ret->player_position_y), sizeof(int), 1, f);
	fclose(f);
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

void load_region_assets(region *r)
{
	int i;
	for (i = 0; i < r->numentities; i++) {
		entity *e = &(r->entities[i]);
		spawn_entity(e);
		e->weapon = make_weapon(COLOR_GREEN, &(e->x), &(e->y), 8, 8, 16, 8, 100.0, 0, 1, 8, "sfx/laser.wav");
	}
	for (i = 0; i < r->numlights; i++) {
		lightsource *l = &(r->lights[i]);
		spawn_lightsource(l);
	}
	for (i = 0; i < r->numfx; i++) {
		effect *fx = &(r->fx[i]);
		spawn_fx(fx);
	}
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
	ret->numentities = 0;
	ret->numlights = 0;
	ret->numfx = 0;
	return ret;
}

void add_standard_room(region *r)
{
	STANDARD_ROOMS[STANDARD_ROOMS_INDEX++] = r;
}

void populate_region(region *r, int rtype)
{
	r->width = STANDARD_ROOMS[rtype]->width;
	r->height = STANDARD_ROOMS[rtype]->height;
	r->ambience = STANDARD_ROOMS[rtype]->ambience;
	r->numentities = STANDARD_ROOMS[rtype]->numentities;
	r->numlights = STANDARD_ROOMS[rtype]->numlights;
	r->numfx = STANDARD_ROOMS[rtype]->numfx;
	memcpy(r->blocks, STANDARD_ROOMS[rtype]->blocks, sizeof(r->blocks));
	memcpy(r->entities, STANDARD_ROOMS[rtype]->entities, sizeof(r->entities));
	memcpy(r->lights, STANDARD_ROOMS[rtype]->lights, sizeof(r->lights));
	memcpy(r->fx, STANDARD_ROOMS[rtype]->fx, sizeof(r->fx));
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
