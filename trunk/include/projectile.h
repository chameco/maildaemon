#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
#include "character.h"
#include "level.h"
#include "enemy.h"
#include "fx.h"
typedef struct projectile {
	int r, g, b, a;
	int x, y, dim;
	int xv, yv;
	void *sb; //Spawned by.
	int dmg;
	int edim; //Effect dimension.
} projectile;
void initialize_projectiles();
void reset_projectiles();
void spawn_projectile(double r, double g, double b, double a,
		direction d, int x, int y, int dim,
		void *sb, int dmg, int speed, int edim);
void check_projectile_collisions(projectile *p);
void destroy_projectile(projectile *p);
void draw_projectile(projectile *p);
void update_projectiles();
void draw_projectiles();
#endif
