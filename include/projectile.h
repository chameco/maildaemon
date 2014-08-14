#pragma once

#include "utils.h"
#include "texture.h"
#include "item.h"

typedef struct projectile {
	texture *t;
	double x, y;
	int w, h;
	double speed;
	double rotation;
	double xv, yv;
	int longevity;
	item *spawned_by;
	int dmg;
} projectile;

void initialize_projectile();
void reset_projectile();
projectile *build_projectile_prototype(char *name, double speed, int w, int h, int longevity, int dmg);
projectile *make_projectile(char *name, double x, double y, double rotation, item *spawned_by);
void spawn_projectile(projectile *e);
void check_projectile_collisions(projectile *p);
void destroy_projectile(projectile *p);
void update_projectile();
void draw_projectile();
