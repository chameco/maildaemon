#ifndef LEVEL_H
#define LEVEL_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "lights.h"
#include "entity.h"
#include "player.h"
#include "enemy.h"
#include "projectile.h"
#include "fx.h"
#include "utils.h"

typedef enum block {
	VOID,
	FLOOR,
	WALL,
	TORCH
} block;

typedef struct level {
	char name[256];
	int width;
	int height;
	block dimensions[50][50];
} level;

level *get_current_level();
int get_current_level_index();
char *get_current_level_name();
double get_current_level_ambience();
int get_block_dim();
int is_solid_block(block b);
void initialize_levels();
void load_level(int index);
void build_level_lightmap();
void draw_block(block b, int x, int y);
void draw_current_level();
#endif
