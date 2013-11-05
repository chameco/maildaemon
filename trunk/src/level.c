#include "level.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <cuttle/debug.h>
#include <cuttle/utils.h>
#include "utils.h"
#include "resources.h"
#include "worldgen.h"
#include "lights.h"
#include "entity.h"
#include "player.h"
#include "projectile.h"
#include "fx.h"

world *WORLD;

resource *BLOCK_RESOURCES[256];
const int BLOCK_DIM = 32;

world *get_world()
{
	return WORLD;
}

int get_block_dim()
{
	return BLOCK_DIM;
}

void initialize_level()
{
	BLOCK_RESOURCES[VOID] = load_resource("textures/blank.png");
	BLOCK_RESOURCES[SNOW] = load_resource("textures/snow.png");
	BLOCK_RESOURCES[GRASS] = load_resource("textures/grass.png");
	BLOCK_RESOURCES[SAND] = load_resource("textures/sand.png");
	BLOCK_RESOURCES[PLANKS] = load_resource("textures/floor.png");
	BLOCK_RESOURCES[STONE] = load_resource("textures/wall.png");
	BLOCK_RESOURCES[SHRUB] = load_resource("textures/shrub.png");
	BLOCK_RESOURCES[TORCH] = load_resource("textures/torch.png");
	WORLD = load_world();
}

void change_current_region(int rx, int ry, direction d)
{
	region *dest_region;
	dest_region = get_region(WORLD, rx, ry);
	int x = get_player_x();
	int y = get_player_y();
	unload_region(WORLD, rx, ry);
	load_region(WORLD, rx, ry);
	if (d == NORTH) {
		warp_player(x, get_block_dim() * (dest_region->height - 2));
	} else if (d == SOUTH) {
		warp_player(x, get_block_dim());
	} else if (d == WEST) {
		warp_player(get_block_dim() * (dest_region->width - 2), y);
	} else if (d == EAST) {
		warp_player(get_block_dim(), y);
	}
	reset_lights();
	reset_entities();
	reset_projectile();
	reset_fx();
}

void update_level()
{
	int x, y, w, h;
	x = get_player_x();
	y = get_player_y();
	w = get_player_w();
	h = get_player_h();
	int region_x, region_y;
	region_x = get_current_x_coord(WORLD);
	region_y = get_current_y_coord(WORLD);
	int randx, randy;
	if (rand() % 16 == 0) {
		if (get_current_region(WORLD)->humidity >= 0.5) {
			randx = rand() % get_current_region(WORLD)->width;
			randy = rand() % get_current_region(WORLD)->height;
			if (!is_solid_block(get_current_region(WORLD), randx, randy)) {
				spawn_entity(0, randx * get_block_dim(), randy * get_block_dim(), 32, 32,
						make_weapon(COLOR_GREEN, NULL, NULL, 8, 8, 8, 16, 16, 100.0, 0, 1, "sfx/laser.wav"),
						10, 0, 10.0);
			}
		}
	}
	if (x <= 0 && region_x != 0) {
		change_current_region(region_x-1, region_y, WEST);
	} else if (x+w >= get_block_dim() * get_current_region(WORLD)->width && region_x != WORLD_DIM - 1) {
		change_current_region(region_x+1, region_y, EAST);
	}
	if (y <= 0 && region_y != 0) {
		change_current_region(region_x, region_y-1, NORTH);
	} else if (y+h >= get_block_dim() * get_current_region(WORLD)->height && region_y != WORLD_DIM - 1) {
		change_current_region(region_x, region_y+1, SOUTH);
	}
}

void draw_level()
{
	int x, y;
	for (x = 0; x < get_current_region(WORLD)->width; x++) {
		for (y = 0; y < get_current_region(WORLD)->height; y++) {
			draw_resource(BLOCK_RESOURCES[get_current_region(WORLD)->blocks[x][y]],
					x*BLOCK_DIM,
					y*BLOCK_DIM);
		}
	}
}
