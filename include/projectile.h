#pragma once

#include "utils.h"
#include "item.h"

typedef struct projectile {
	color c;
	int x, y, w, h;
	int xv, yv;
	int longevity;
	item *spawned_by;
	int dmg;
} projectile;

void initialize_projectile();
void reset_projectile();
void spawn_projectile(color c, int x, int y, int speed, double rotation, int w, int h,
		int longevity, item *spawned_by, int dmg);
void check_projectile_collisions(projectile *p);
void destroy_projectile(projectile *p);
void update_projectile();
void draw_one_projectile(projectile *p);
void draw_projectile();
