#include "level.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "resources.h"
#include "lights.h"
#include "entity.h"
#include "player.h"
#include "projectile.h"
#include "fx.h"

static level *CURRENT_LEVEL = NULL;
static resource *TILE_RESOURCES[256];

void initialize_level()
{
	TILE_RESOURCES[VOID] = load_resource("textures/blank.png");
	TILE_RESOURCES[SNOW] = load_resource("textures/snow.png");
	TILE_RESOURCES[GRASS] = load_resource("textures/grass.png");
	TILE_RESOURCES[SAND] = load_resource("textures/sand.png");
	TILE_RESOURCES[PLANKS] = load_resource("textures/floor.png");
	TILE_RESOURCES[STONE] = load_resource("textures/wall.png");
	TILE_RESOURCES[SHRUB] = load_resource("textures/shrub.png");
	TILE_RESOURCES[TORCH] = load_resource("textures/torch.png");
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
			draw_resource(TILE_RESOURCES[CURRENT_LEVEL->tiles[x][y]],
					x*TILE_DIM,
					y*TILE_DIM);
		}
	}
}
