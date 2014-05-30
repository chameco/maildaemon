#include "level.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "texture.h"

static level *CURRENT_LEVEL = NULL;
static texture *TILE_RESOURCES[256];

void initialize_level()
{
	TILE_RESOURCES[VOID] = load_texture("textures/blank.png", 32, 32);
	TILE_RESOURCES[SNOW] = load_texture("textures/snow.png", 32, 32);
	TILE_RESOURCES[GRASS] = load_texture("textures/grass.png", 32, 32);
	TILE_RESOURCES[SAND] = load_texture("textures/sand.png", 32, 32);
	TILE_RESOURCES[PLANKS] = load_texture("textures/floor.png", 32, 32);
	TILE_RESOURCES[STONE] = load_texture("textures/wall.png", 32, 32);
	TILE_RESOURCES[SHRUB] = load_texture("textures/shrub.png", 32, 32);
	TILE_RESOURCES[TORCH] = load_texture("textures/torch.png", 32, 32);
}

void switch_level(char *name)
{
	CURRENT_LEVEL = load_level(name);
}

level *load_level(char *name)
{
	level *ret = malloc(sizeof(level));
	char buf[256] = "levels/";
	strcat(buf, name);
	int bare = strlen(buf);
	strcat(buf, ".lvl");
	FILE *f = fopen(buf, "r");
	if (f == NULL) {
		debug("null");
		int x, y;
		for (x = 0; x < LEVEL_MAX_DIM; x++) {
			for (y = 0; y < LEVEL_MAX_DIM; y++) {
				ret->tiles[x][y] = PLANKS;
			}
		}
		strcpy(ret->name, "new");
		ret->ambience = 0.75;
	} else {
		fread(ret, sizeof(level), 1, f);
		fclose(f);
		buf[bare] = 0x0;
		strcat(buf, ".scm");
		scm_c_primitive_load(buf);
	}
	return ret;
}

void save_level(level *l)
{
	char buf[256] = "levels/";
	strncat(buf, l->name, sizeof(buf) - strlen(buf) - 5);
	strcat(buf, ".lvl");
	FILE *f = fopen(buf, "w+");
	fwrite(l, sizeof(level), 1, f);
	fclose(f);
}

level *get_current_level()
{
	return CURRENT_LEVEL;
}

int is_solid_tile(int x, int y)
{
	if (x < 0 || y < 0) return 0;
	return CURRENT_LEVEL->tiles[x][y] > VOID;
}

void draw_level()
{
	int x, y;
	for (x = 0; x < LEVEL_MAX_DIM; x++) {
		for (y = 0; y < LEVEL_MAX_DIM; y++) {
			draw_texture(TILE_RESOURCES[CURRENT_LEVEL->tiles[x][y]],
					x*TILE_DIM,
					y*TILE_DIM);
		}
	}
}
