#ifndef ENEMY_H
#define ENEMY_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
#include "player.h"
#include "level.h"
#include "projectile.h"

#define SPAWN_SLIME(x, y) spawn_enemy(0, x, y, 32, 32, \
		10, 8, 2, 10, 1, 16, \
		10, COLOR_GREEN, 8, 4, 50)

#define SPAWN_WIZARD(x, y) spawn_enemy(1, x, y, 32, 32, \
		100, 8, 10, 100, 4, 8, \
		15, COLOR_WHITE, 8, 4, 50)

typedef struct enemy {
	int id;
	int x, y, w, h;
	int health, speed, attk;
	int firing;
	int firingdirecx, firingdirecy;
	int cooldown;
	int cooldowncounter;
	int pcount;
	int pspeed;
	color pc;
	int pdim, edim;
	int eradius;
	double expval;
} enemy;

list_node *get_enemies();
void set_enemies(list_node *enemies);
void initialize_one_enemy(int i, char *path, int w, int h);
void initialize_enemy();
void reset_enemies();
void spawn_enemy(int id, int x, int y, int w, int h,
		int health, int speed, int attk, double expval, int pcount, int pspeed, 
		int cooldown, color pc, int pdim, int edim, int eradius);
void hit_enemy();
void collide_enemy(enemy *e);
void shoot_enemy_weapon(enemy *e, int xv, int yv);
void move_enemy_north(enemy *e);
void move_enemy_south(enemy *e);
void move_enemy_west(enemy *e);
void move_enemy_east(enemy *e);
void move_enemy_worker(enemy *e, SDL_Rect a);
void update_enemy();
void draw_one_enemy();
void draw_enemy();
#endif
