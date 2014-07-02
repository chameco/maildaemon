#pragma once

#include <cuttle/utils.h>
#include <libguile.h>
#include "utils.h"
#include "texture.h"
#include "item.h"

typedef struct entity {
	double x, y;
	int w, h;
	double xv, yv;
	texture *t;
	item *item;
	int health;
	double speed;
	double expval;
	SCM data;
	SCM init_func;
	SCM hit_func;
	SCM collide_func;
	SCM update_func;
} entity;

void initialize_entity();
list_node *get_entities();
void reset_entity();
void free_entity(entity *e);
entity *build_entity_prototype(char *name, int w, int h,
		int health, double speed, double expval);
void set_entity_init(entity *e, SCM init);
void set_entity_hit(entity *e, SCM hit);
void set_entity_collide(entity *e, SCM collide);
void set_entity_update(entity *e, SCM update);
entity *make_entity(char *name, double x, double y);
void spawn_entity(entity *e);
void hit_entity(entity *e, int dmg);
void collide_entity(entity *e);
void give_entity_item(entity *e, item *i);
void move_entity(entity *e, direction d);
void update_entity();
void draw_entity();
