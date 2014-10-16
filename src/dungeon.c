#include "dungeon.h"

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

static char CURRENT_DUNGEON[256] = {0};

void __api_set_dungeon(solid_vm *vm)
{
	char *s = solid_get_str_value(solid_pop_stack(vm));
	set_dungeon(s);
}

void __api_get_dungeon(solid_vm *vm)
{
	vm->regs[255] = solid_str(vm, get_dungeon());
}

void initialize_dungeon()
{
	defun("set_dungeon", __api_set_dungeon);
	defun("get_dungeon", __api_get_dungeon);
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

solid_object *dungeon_load(char *path)
{
	char buf[256];
	sprintf(buf, "dungeons/%s/%s", CURRENT_DUNGEON, path);
	return load(buf);
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
