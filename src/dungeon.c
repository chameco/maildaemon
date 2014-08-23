#include "dungeon.h"

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

static char CURRENT_DUNGEON[256] = {0};

SCM __api_set_dungeon(SCM d)
{
	char *s = scm_to_locale_string(d);
	set_dungeon(s);
	free(s);
	return SCM_BOOL_F;
}

SCM __api_get_dungeon()
{
	return scm_from_locale_string(get_dungeon());
}

void initialize_dungeon()
{
	scm_c_define_gsubr("set-dungeon", 1, 0, 0, __api_set_dungeon);
	scm_c_define_gsubr("get-dungeon", 0, 0, 0, __api_get_dungeon);
}

void set_dungeon(char *dungeon)
{
	strcpy(CURRENT_DUNGEON, dungeon);
	dungeon_load_all("script/entities");
}

char *get_dungeon()
{
	return CURRENT_DUNGEON;
}

texture *dungeon_load_texture(char *path, int w, int h)
{
	char buf[256];
	sprintf(buf, "dungeons/%s/%s", CURRENT_DUNGEON, path);
	return load_texture(buf, w, h);
}

SCM dungeon_load(char *path)
{
	char buf[256];
	sprintf(buf, "dungeons/%s/%s", CURRENT_DUNGEON, path);
	return scm_c_primitive_load(buf);
}

void dungeon_load_all(char *dir)
{
	char buf[256];
	sprintf(buf, "dungeons/%s/%s", CURRENT_DUNGEON, dir);
	load_all(buf);
}

FILE *dungeon_fopen(char *path, char *mode)
{
	char buf[256];
	sprintf(buf, "dungeons/%s/%s", CURRENT_DUNGEON, path);
	return fopen(buf, mode);
}
