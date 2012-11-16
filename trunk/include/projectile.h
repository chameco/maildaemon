#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
#include "character.h"
#include "enemy.h"
#include "level.h"
typedef struct projectile {
	int r, g, b, a;
	int x, y, w, h;
	int xv, yv;
	int sbp; //Spawned by player
	int dmg;
} projectile;
void initialize_projectiles();
void spawn_projectile(double r, double g, double b, double a,
		direction d, int x, int y, int w, int h,
		int sbp, int dmg, int speed);
void check_projectile_collisions(projectile *p);
void destroy_projectile(projectile *p);
void draw_projectile(projectile *p);
void update_projectiles();
void draw_projectiles();
#endif
