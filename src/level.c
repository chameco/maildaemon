#include "level.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "texture.h"

static level *CURRENT_LEVEL = NULL;
static texture *TILE_SHEET = NULL;

void initialize_level()
{
	TILE_SHEET = load_texture("textures/tiles/sheet.png", 8, 8);
}

void reset_level()
{
	switch_level(CURRENT_LEVEL->name);
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
	return CURRENT_LEVEL->tiles[x][y] > VOID;
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
