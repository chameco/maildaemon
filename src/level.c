#include "level.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "texture.h"
#include "entity.h"
#include "projectile.h"
#include "fx.h"
#include "lightsource.h"

static level *CURRENT_LEVEL = NULL;
static texture *TILE_SHEET = NULL;
static texture *TILE_SHEET_TOP = NULL;
static char SWITCH_TO[256] = {0};

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
	scm_c_define_gsubr("save-level", 0, 0, 0, __api_save_level);

	DIR *d = opendir("script/levels/helpers");
	struct dirent *entry;
	char buf[256];
	if (d != NULL) {
		while ((entry = readdir(d))) {
			char *pos = strrchr(entry->d_name, '.') + 1;
			if (pos != NULL && strcmp(pos, "scm") == 0) {
				strcpy(buf, "script/levels/helpers/");
				strcat(buf, entry->d_name);
				scm_c_primitive_load(buf);
			}
		}
		closedir(d);
	} else {
		log_err("Directory \"script/levels/helpers/\" does not exist");
		exit(1);
	}
}

void reset_level()
{
	switch_level(CURRENT_LEVEL->name);
}

void switch_level(char *name)
{
	strcpy(SWITCH_TO, name);
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
		scm_c_primitive_load(sbuf);
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
	return CURRENT_LEVEL->tiles[x][y] >= STONE;
}

void update_level()
{
	if (strlen(SWITCH_TO) != 0) {
		reset_entity();
		reset_projectile();
		reset_fx();
		reset_lightsource();
		CURRENT_LEVEL = load_level(SWITCH_TO);
		memset(SWITCH_TO, 0, sizeof(SWITCH_TO));
	}
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
