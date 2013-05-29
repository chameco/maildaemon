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
#include "player.h"
#include "entity.h"
#include "level.h"
#include "enemy.h"
#include "fx.h"
typedef struct projectile {
	color c;
	int x, y, w, h;
	int xv, yv;
	int longevity;
	void *sb; //Spawned by.
	int dmg;
	int edim; //Effect dimension.
	GLuint texture;
} projectile;
void initialize_projectile();
void reset_projectile();
void spawn_projectile(color c, int x, int y, int xv, int yv, int w, int h,
		int longevity, void *sb, int dmg, int edim, GLuint texture);
void check_projectile_collisions(projectile *p);
void destroy_projectile(projectile *p);
void update_projectile();
void draw_one_projectile(projectile *p);
void draw_projectile();
#endif
