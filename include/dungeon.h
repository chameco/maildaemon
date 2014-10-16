#pragma once

#include <stdio.h>

#include <solid/solid.h>

#include "texture.h"

void initialize_dungeon();

void set_dungeon(char *dungeon);
char *get_dungeon();

texture *dungeon_load_texture(char *path, int w, int h);
solid_object *dungeon_load(char *path);
void dungeon_load_all(char *dir);
FILE *dungeon_fopen(char *path, char *mode);
