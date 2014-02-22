#ifndef PURGE_ENTITY_H
#define PURGE_ENTITY_H
#include <cuttle/utils.h>
#include <solid/solid.h>
#include "utils.h"
#include "weapon.h"

typedef struct entity {
	int id;
	int x, y, w, h;
	int xv, yv;
	weapon *weapon;
	int health, speed;
	double expval;
	solid_object *on_hit;
	solid_object *on_collide;
	solid_object *on_update;
	int freeable;
} entity;

void initialize_entity();
list_node *get_entities();
void set_entities(list_node *entities);
void reset_entities();
entity *make_entity(int id, int x, int y, int w, int h,
		weapon *weapon, int health, int speed, double expval);
void spawn_entity(entity *e);
void hit_entity(entity *e, int dmg);
void collide_entity(entity *e);
void move_entity(entity *e, direction d);
void update_entity();
void draw_entity();
#endif
