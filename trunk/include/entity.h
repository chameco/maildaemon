#ifndef PURGE_ENTITY_H
#define PURGE_ENTITY_H
#include <cuttle/utils.h>
#include "utils.h"
#include "weapon.h"

typedef struct entity {
	int id;
	int x, y, w, h;
	weapon *weapon;
	int health, speed;
	double expval;
} entity;

list_node *get_entities();
void set_entities(list_node *entities);
void initialize_entity();
void reset_entities();
void spawn_entity(int id, int x, int y, int w, int h,
		weapon *weapon, int health, int speed, double expval);
void hit_entity();
void collide_entity(entity *e);
void move_entity(entity *e, direction d);
void update_entity();
void draw_entity();
#endif
