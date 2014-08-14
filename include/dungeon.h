#pragma once

#include <stdio.h>

#include <libguile.h>

void initialize_dungeon();

void set_dungeon(char *dungeon);
char *get_dungeon();

SCM dungeon_load(char *path);
void dungeon_load_all(char *dir);
FILE *dungeon_fopen(char *path, char *mode);
