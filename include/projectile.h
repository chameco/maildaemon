#pragma once

#include "utils.h"
#include "texture.h"
#include "item.h"

typedef struct projectile {
	texture *t;
	int x, y, w, h;
	int speed;
	double rotation;
	int xv, yv;
	int longevity;
	item *spawned_by;
	int dmg;
} projectile;

void initialize_projectile();
void reset_projectile();
void build_projectile_prototype(char *name, int speed, int w, int h, int longevity, int dmg);
void spawn_projectile(char *name, int x, int y, double rotation, item *spawned_by);
void check_projectile_collisions(projectile *p);
void destroy_projectile(projectile *p);
void update_projectile();
void draw_projectile();
