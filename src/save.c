#include "save.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <solid/solid.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "dungeon.h"
#include "game.h"
#include "player.h"
#include "level.h"

void __api_save_game(solid_vm *vm)
{
	save_game(solid_get_int_value(solid_pop_stack(vm)));
}

void __api_load_game(solid_vm *vm)
{
	vm->regs[255] = solid_bool(vm, load_game(solid_get_int_value(solid_pop_stack(vm))));
}

void initialize_save()
{
	defun("save_game", __api_save_game);
	defun("load_game", __api_load_game);
}

void save_game(int slot)
{
	char path[256];
	sprintf(path, "saves/%d.sav", slot);
	FILE *f = fopen(path, "w+");
	savegame s;
	strcpy(s.dungeon_name, get_dungeon());
	strcpy(s.level_name, get_current_level()->name);
	s.player_x = get_current_spawn_x();
	s.player_y = get_current_spawn_y();
	fwrite(&s, sizeof(savegame), 1, f);
	fclose(f);
}

bool load_game(int slot)
{
	char path[256];
	sprintf(path, "saves/%d.sav", slot);
	FILE *f = fopen(path, "r");
	if (f != NULL) {
		savegame *s = (savegame *) malloc(sizeof(savegame));
		fread(s, sizeof(level), 1, f);
		set_dungeon(s->dungeon_name);
		switch_level(s->level_name);
		warp_player(s->player_x, s->player_y);
		fclose(f);
		return true;
	}
	return false;
}
