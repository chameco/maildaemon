#include "save.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "dungeon.h"
#include "game.h"
#include "player.h"
#include "level.h"

SCM __api_save_game(SCM slot)
{
	save_game(scm_to_int(slot));
	return SCM_BOOL_F;
}

SCM __api_load_game(SCM slot)
{
	return scm_from_bool(load_game(scm_to_int(slot)));
}

void initialize_save()
{
	scm_c_define_gsubr("save-game", 1, 0, 0, __api_save_game);
	scm_c_define_gsubr("load-game", 1, 0, 0, __api_load_game);
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
