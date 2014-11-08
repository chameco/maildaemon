#include "level.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#include <solid/solid.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "texture.h"
#include "dungeon.h"
#include "player.h"
#include "entity.h"
#include "projectile.h"
#include "fx.h"
#include "lightsource.h"
#include "scheduler.h"

static level *CURRENT_LEVEL = NULL;
static double CURRENT_SPAWN_X = 0;
static double CURRENT_SPAWN_Y = 0;
static texture *TILE_SHEET = NULL;
static texture *TILE_SHEET_TOP = NULL;

void __api_switch_level(solid_vm *vm)
{
	char *s = solid_get_str_value(solid_pop_stack(vm));
	switch_level(s);
}

void __api_set_tile(solid_vm *vm)
{
	int tile = solid_get_int_value(solid_pop_stack(vm));
	int y = solid_get_int_value(solid_pop_stack(vm));
	int x = solid_get_int_value(solid_pop_stack(vm));
	get_current_level()->tiles[x][y] = tile;
}

void __api_set_level_name(solid_vm *vm)
{
	char *s = solid_get_str_value(solid_pop_stack(vm));
	strcpy(get_current_level()->name, s);
}

void __api_get_level_name(solid_vm *vm)
{
	vm->regs[255] = solid_str(vm, get_current_level()->name);
}

void __api_set_level_ambience(solid_vm *vm)
{
	get_current_level()->ambience = solid_get_double_value(solid_pop_stack(vm));
}

void __api_get_level_ambience(solid_vm *vm)
{
	vm->regs[255] = solid_double(vm, get_current_level()->ambience);
}

void __api_save_level(solid_vm *vm)
{
	save_level(get_current_level());
}

void initialize_level()
{
	TILE_SHEET = load_texture("textures/tilesheet.png", 8, 8);
	TILE_SHEET_TOP = load_texture("textures/tilesheet_top.png", 8, 8);

	defun("switch_level", __api_switch_level);
	defun("set_tile", __api_set_tile);
	defun("set_level_name", __api_set_level_name);
	defun("get_level_name", __api_get_level_name);
	defun("set_level_ambience", __api_set_level_ambience);
	defun("get_level_ambience", __api_get_level_ambience);
	defun("save_level", __api_save_level);

	load_all("script/levels/helpers");
}

void reset_level()
{
	switch_level(CURRENT_LEVEL->name);
}

void check_failure()
{
	debug("check");
	debug("type: %d", solid_get_namespace(solid_get_current_namespace(get_vm()), solid_str(get_vm(), "make_stairs"))->type);
	debug("check succeed");
}

void switch_level(char *name)
{
	reset_entity();
	reset_projectile();
	reset_fx();
	reset_lightsource();
	CURRENT_LEVEL = load_level(name);
	CURRENT_SPAWN_X = get_player_x();
	CURRENT_SPAWN_Y = get_player_y();
}

level *load_level(char *name)
{
	level *ret = malloc(sizeof(level));
	char buf[256] = "levels/";
	strcat(buf, name);
	int bare = strlen(buf);
	strcat(buf, ".lvl");
	FILE *f = dungeon_fopen(buf, "r");
	if (f == NULL) {
		int x, y;
		for (x = 0; x < LEVEL_MAX_DIM; x++) {
			for (y = 0; y < LEVEL_MAX_DIM; y++) {
				ret->tiles[x][y] = PLANKS;
			}
		}
		strcpy(ret->name, name);
		ret->ambience = 0.75;
	} else {
		fread(ret, sizeof(level), 1, f);
		fclose(f);
		buf[bare] = 0x0;
		strcat(buf, ".sol");
		char sbuf[256] = "script/";
		strcat(sbuf, buf);
		dungeon_load(sbuf);
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

double get_current_spawn_x()
{
	return CURRENT_SPAWN_X;
}

double get_current_spawn_y()
{
	return CURRENT_SPAWN_Y;
}

bool is_solid_tile(int x, int y)
{
	if (x < 0 || y < 0) return false;
	if (x >= LEVEL_MAX_DIM || y >= LEVEL_MAX_DIM) return false;
	return CURRENT_LEVEL->tiles[x][y] >= STONE;
}

void draw_level()
{
	int x, y;
	for (x = 0; x < LEVEL_MAX_DIM; x++) {
		for (y = 0; y < LEVEL_MAX_DIM; y++) {
			set_sheet_row(TILE_SHEET, CURRENT_LEVEL->tiles[x][y]/8);
			set_sheet_column(TILE_SHEET, CURRENT_LEVEL->tiles[x][y]%8);
			draw_texture_scale(TILE_SHEET, x*TILE_DIM, y*TILE_DIM, TILE_DIM, TILE_DIM);
		}
	}
}

void draw_level_top()
{
	int x, y;
	for (x = 0; x < LEVEL_MAX_DIM; x++) {
		for (y = 0; y < LEVEL_MAX_DIM; y++) {
			if (!is_solid_tile(x, y-1)) {
				set_sheet_row(TILE_SHEET_TOP, CURRENT_LEVEL->tiles[x][y]/8);
				set_sheet_column(TILE_SHEET_TOP, CURRENT_LEVEL->tiles[x][y]%8);
				draw_texture_scale(TILE_SHEET_TOP, x*TILE_DIM, (y-1)*TILE_DIM, TILE_DIM, TILE_DIM);
			}
		}
	}
}
