#ifndef ENTITY_H
#define ENTITY_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "level.h"
#include "player.h"
#include "enemy.h"
#include "projectile.h"
#include "fx.h"
#include "utils.h"

typedef struct entity {
	int id;
	int x, y, w, h;
} entity;

list_node *get_entities();
void initialize_one_entity(int i, char *path, int w, int h);
void initialize_entity();
void reset_entities();
void spawn_entity(int id, int x, int y, int w, int h);
int collide_entity(entity *e);
void hit_entity(entity *e, int dmg);
void update_entity();
void draw_one_entity(entity *e);
void draw_entity();
#endif
