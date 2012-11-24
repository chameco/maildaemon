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
#include "level.h"
#include "enemy.h"
typedef struct projectile {
	int r, g, b, a;
	int x, y, w, h;
	int xv, yv;
	void *sb; //Spawned by.
	int dmg;
} projectile;
void initialize_projectiles();
void reset_projectiles();
void spawn_projectile(double r, double g, double b, double a,
		direction d, int x, int y, int w, int h,
		void *sbp, int dmg, int speed);
void check_projectile_collisions(projectile *p);
void destroy_projectile(projectile *p);
void draw_projectile(projectile *p);
void update_projectiles();
void draw_projectiles();
#endif
