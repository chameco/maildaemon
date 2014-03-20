#include "level.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "vm.h"
#include "utils.h"
#include "resources.h"
#include "lights.h"
#include "entity.h"
#include "player.h"
#include "projectile.h"
#include "fx.h"

level *CURRENT_LEVEL = NULL;
resource *TILE_RESOURCES[256];

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
	CURRENT_LEVEL = load_level(NULL);
}

level *load_level(char *path)
{
	level *ret = (level *) malloc(sizeof(level));
	int x, y;
	for (x = 0; x < LEVEL_MAX_DIM; x++) {
		for (y = 0; y < LEVEL_MAX_DIM; y++) {
			ret->tiles[x][y] = PLANKS;
		}
	}
	ret->name = "test";
	return ret;
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
