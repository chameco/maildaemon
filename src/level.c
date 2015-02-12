#include "level.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#include <libguile.h>

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
#include "sight.h"
#include "scheduler.h"

static level *CURRENT_LEVEL = NULL;
static double CURRENT_SPAWN_X = 0;
static double CURRENT_SPAWN_Y = 0;
static texture *TILE_SHEET = NULL;
static texture *TILE_SHEET_TOP = NULL;

SCM __api_switch_level(SCM name)
{
	char *s = scm_to_locale_string(name);
	switch_level(s);
	free(s);
	return SCM_BOOL_F;
}

SCM __api_set_tile(SCM x, SCM y, SCM tile)
{
	get_current_level()->tiles[scm_to_int(x)][scm_to_int(y)] = scm_to_int(tile);
	return SCM_BOOL_F;
}

SCM __api_set_level_name(SCM name)
{
	char *s = scm_to_locale_string(name);
	strcpy(get_current_level()->name, s);
	free(s);
	return SCM_BOOL_F;
}

SCM __api_get_level_name()
{
	return scm_from_locale_string(get_current_level()->name);
}

SCM __api_set_level_ambience(SCM a)
{
	get_current_level()->ambience = scm_to_double(a);
	return SCM_BOOL_F;
}

SCM __api_get_level_ambience()
{
	return scm_from_double(get_current_level()->ambience);
}

SCM __api_save_level()
{
	save_level(get_current_level());
	return SCM_BOOL_F;
}

void initialize_level()
{
	TILE_SHEET = load_texture("textures/tilesheet.png", 8, 8);
	TILE_SHEET_TOP = load_texture("textures/tilesheet_top.png", 8, 8);

	scm_c_define_gsubr("switch-level", 1, 0, 0, __api_switch_level);
	scm_c_define_gsubr("set-tile", 3, 0, 0, __api_set_tile);
	scm_c_define_gsubr("set-level-name", 1, 0, 0, __api_set_level_name);
	scm_c_define_gsubr("get-level-name", 0, 0, 0, __api_get_level_name);
	scm_c_define_gsubr("set-level-ambience", 1, 0, 0, __api_set_level_ambience);
	scm_c_define_gsubr("get-level-ambience", 0, 0, 0, __api_get_level_ambience);
	scm_c_define_gsubr("save-level", 0, 0, 0, __api_save_level);

	load_all("script/levels/helpers");
}

void reset_level()
{
	switch_level(CURRENT_LEVEL->name);
}

void switch_level(char *name)
{
	reset_entity();
	reset_projectile();
	reset_fx();
	reset_lightsource();
	CURRENT_LEVEL = load_level(name);
	reset_sight();
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
		strcat(buf, ".scm");
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
	FILE *f = dungeon_fopen(buf, "w+");
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
			if (CURRENT_LEVEL->tiles[x][y] != VOID) {
				set_sheet_row(TILE_SHEET, CURRENT_LEVEL->tiles[x][y]/8);
				set_sheet_column(TILE_SHEET, CURRENT_LEVEL->tiles[x][y]%8);
				draw_texture_scale(TILE_SHEET, x*TILE_DIM, y*TILE_DIM, TILE_DIM, TILE_DIM);
			}
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
