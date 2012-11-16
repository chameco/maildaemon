#ifndef LEVEL_H
#define LEVEL_H
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "character.h"
#include "enemy.h"
#include "utils.h"

typedef enum block {
	FLOOR,
	WALL
} block;

typedef struct level {
	char name[256];
	int width;
	int height;
	block dimensions[50][50];
} level;

level *get_current_level();
int get_block_dim();
int is_solid_block();
void initialize_levels();
void load_level(int index);
void draw_block(block b, int x, int y);
void draw_current_level();
#endif
