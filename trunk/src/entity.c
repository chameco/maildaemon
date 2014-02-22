#include "entity.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>
#include <solid/solid.h>

#include "vm.h"
#include "utils.h"
#include "resources.h"
#include "worldgen.h"
#include "level.h"
#include "fx.h"
#include "ai.h"
#include "player.h"
#include "weapon.h"

resource *ENTITY_RESOURCES[256];
list_node *ENTITIES;

void PAPI_make_entity(solid_vm *vm)
{
	double expval = solid_get_double_value(solid_pop_stack(vm));
	int speed = solid_get_int_value(solid_pop_stack(vm));
	int health = solid_get_int_value(solid_pop_stack(vm));
	weapon *weap = (weapon *) solid_get_struct_value(solid_pop_stack(vm));
	int h = solid_get_int_value(solid_pop_stack(vm));
	int w = solid_get_int_value(solid_pop_stack(vm));
	int y = solid_get_int_value(solid_pop_stack(vm));
	int x = solid_get_int_value(solid_pop_stack(vm));
	int id = solid_get_int_value(solid_pop_stack(vm));
	vm->regs[255] = solid_struct(vm, make_entity(id, x, y, w, h, weap, health, speed, expval));
}

PAPI_field_from_struct(solid_int, entity, id)
PAPI_field_from_struct(solid_int, entity, x)
PAPI_field_from_struct(solid_int, entity, y)
PAPI_field_from_struct(solid_int, entity, w)
PAPI_field_from_struct(solid_int, entity, h)
PAPI_field_from_struct(solid_int, entity, xv)
PAPI_field_from_struct(solid_int, entity, yv)

PAPI_field_from_struct(solid_struct, entity, weapon)

PAPI_field_from_struct(solid_int, entity, health)
PAPI_field_from_struct(solid_int, entity, speed)

PAPI_field_from_struct(solid_double, entity, expval)

PAPI_getter(solid_list, get_entities)
PAPI_1param(set_entities, solid_get_list_value)

PAPI_1param(spawn_entity, solid_get_struct_value)

PAPI_2param(hit_entity, solid_get_struct_value, solid_get_int_value)
PAPI_1param(collide_entity, solid_get_struct_value)
PAPI_2param(move_entity, solid_get_struct_value, solid_get_int_value)

void initialize_entity_api()
{
	solid_object *ns = make_namespace("Entity");
	define_function(ns, "get_entity_id", PAPI_entity_id);
	define_function(ns, "get_entity_x", PAPI_entity_x);
	define_function(ns, "get_entity_y", PAPI_entity_y);
	define_function(ns, "get_entity_w", PAPI_entity_w);
	define_function(ns, "get_entity_h", PAPI_entity_h);
	define_function(ns, "get_entity_xv", PAPI_entity_xv);
	define_function(ns, "get_entity_yv", PAPI_entity_yv);

	define_function(ns, "get_entity_weapon", PAPI_entity_weapon);

	define_function(ns, "get_entity_expval", PAPI_entity_expval);

	define_function(ns, "get_entities", PAPI_get_entities);
	define_function(ns, "set_entities", PAPI_set_entities);

	define_function(ns, "make_entity", PAPI_make_entity);
	define_function(ns, "spawn_entity", PAPI_spawn_entity);

	define_function(ns, "hit_entity", PAPI_hit_entity);
	define_function(ns, "collide_entity", PAPI_collide_entity);
	define_function(ns, "move_entity", PAPI_move_entity);
}

void initialize_entity()
{
	ENTITIES = make_list();
	ENTITY_RESOURCES[0] = load_resource("textures/entities/slime.png");
	ENTITY_RESOURCES[1] = load_resource("textures/entities/wizard.png");
	ENTITY_RESOURCES[2] = load_resource("textures/entities/bossblob.png");
	ENTITY_RESOURCES[3] = load_resource("textures/entities/target.png");

	initialize_entity_api();
}

list_node *get_entities()
{
	return ENTITIES;
}

void set_entities(list_node *entities)
{
	ENTITIES = entities;
}

void reset_entities()
{
	list_node *c;
	for (c = ENTITIES->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((entity *) c->data) != NULL) {
			if (((entity *) c->data)->freeable == 1) {
				free((entity *) c->data);
			}
		}
	}
	ENTITIES = make_list();
}

entity *make_entity(int id, int x, int y, int w, int h,
		weapon *weapon, int health, int speed, double expval)
{
	entity *e = (entity *) malloc(sizeof(entity));
	e->id = id;
	e->x = x;
	e->y = y;
	e->w = w;
	e->h = h;
	e->xv = e->yv = 0;
	e->weapon = weapon;
	if (e->weapon != NULL) {
		e->weapon->x = &(e->x);
		e->weapon->y = &(e->y);
	}
	e->health = health;
	e->speed = speed;
	e->expval = expval;
	e->on_hit = NULL;
	e->on_collide = NULL;
	e->on_update = NULL;
	e->freeable = 1;
	return e;
}

void spawn_entity(entity *e)
{
	insert_list(ENTITIES, (void *) e);
}

void hit_entity(entity *e, int dmg)
{
	if (e->on_hit != NULL) {
		solid_call_func(get_vm(), e->on_hit);
	} else {
		e->health -= dmg;
		if (e->health <= 0) {
			remove_list(ENTITIES, (void *) e);
			give_player_exp(e->expval);
			spawn_fx(make_fx(EXPLOSION, e->weapon->c,
						e->x, e->y, 4, 100, 20));
			if (e->freeable == 1) {
				free(e);
			}
		}
	}
}

void collide_entity(entity *e)
{
	if (e->on_collide != NULL) {
		solid_call_func(get_vm(), e->on_collide);
	}
}

void move_entity(entity *e, direction d)
{
	switch (d) {
		case NORTH:
			e->xv = 0;
			e->yv = -e->speed;
			break;
		case SOUTH:
			e->xv = 0;
			e->yv = e->speed;
			break;
		case WEST:
			e->xv = -e->speed;
			e->yv = 0;
			break;
		case EAST:
			e->xv = e->speed;
			e->yv = 0;
			break;
	}
}

void check_entity_collisions(entity *e)
{
	SDL_Rect a, b;
	a.x = e->x;
	a.y = e->y;
	a.w = e->w;
	a.h = e->h;

	b.x = get_player_x();
	b.y = get_player_y();
	b.w = get_player_w();
	b.h = get_player_h();

	if (!check_collision(a, b)) {
		collide_entity(e);
		return;
	}

	int tempx = e->x + e->xv;
	int tempy = e->y + e->yv;

	int shouldmovex = 1;
	int shouldmovey = 1;

	int blockdim = get_block_dim();
	int xmin = (e->x/blockdim)-2;
	int ymin = (e->y/blockdim)-2;
	int xmax = (e->x/blockdim)+2;
	int ymax = (e->y/blockdim)+2;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_block(get_current_region(get_world()), x, y)) {
				b.x = x*32;
				b.y = y*32;
				b.w = b.h = blockdim;
				a.x = tempx;
				a.y = e->y;
				shouldmovex = check_collision(a, b);
				a.x = e->x;
				a.y = tempy;
				shouldmovey = check_collision(a, b);
				a.y = e->y;
			}
		}
	}
	if (shouldmovex) {
		e->x = tempx;
	}
	if (shouldmovey) {
		e->y = tempy;
	}
}

void update_entity()
{
	list_node *c;
	entity *e;
	for (c = ENTITIES->next; c->next != NULL; c = c->next) {
		if (((entity *) c->data) != NULL) {
			e = (entity *) c->data;
			check_entity_collisions(e);
			if (e->on_update != NULL) {
				solid_call_func(get_vm(), e->on_update);
			}
		}
	}
}

void draw_entity()
{
	entity *e;
	list_node *c;
	for (c = ENTITIES; c->next != NULL; c = c->next) {
		if (((entity *) c->data) != NULL) {
			e = (entity *) c->data;
			draw_resource(ENTITY_RESOURCES[e->id], e->x, e->y);
		}
	}
}
