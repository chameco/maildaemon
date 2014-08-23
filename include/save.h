#pragma once

#include <stdbool.h>

typedef struct savegame {
	char dungeon_name[256];
	char level_name[256];
	double player_x, player_y;
} savegame;

void initialize_save();

void save_game(int slot);
bool load_game(int slot);
