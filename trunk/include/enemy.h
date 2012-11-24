#ifndef ENEMY_H
#define ENEMY_H
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
#include "character.h"
#include "level.h"
#include "projectile.h"
typedef struct enemy {
	int x;
	int y;
	int w;
	int h;
	int tex;
	int health;
	int speed;
	int attk;
	double expval;
} enemy;
list_node *get_enemies();
void set_enemies(list_node *enemies);
void initialize_enemies();
void reset_enemies();
void spawn_enemy(int x, int y, int w, int h, int tex, int health, int speed, int attk, double expval);
void hit_enemy();
void collide_enemy(enemy *e);
void move_enemy_north(enemy *e);
void move_enemy_south(enemy *e);
void move_enemy_west(enemy *e);
void move_enemy_east(enemy *e);
void move_enemy_worker(enemy *e, SDL_Rect a);
void update_enemies();
void draw_enemy();
void draw_enemies();
#endif
