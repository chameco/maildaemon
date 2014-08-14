#pragma once

typedef struct savegame {
	char dungeon_name[256];
	char level_name[256];
	double player_x, player_y;
	double player_exp, player_exp_to_next;
	double player_level;
} savegame;

void initialize_save();

void save_game(int slot);
void load_game(int slot);
