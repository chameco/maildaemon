#include "entity.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include <GL/glew.h>
#include <solid/solid.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "dungeon.h"
#include "texture.h"
#include "level.h"
#include "fx.h"
#include "player.h"
#include "item.h"

static list_node *ENTITIES;

static hash_map *ENTITY_PROTOTYPES;

void __api_build_entity_prototype(solid_vm *vm)
{
	double expval = solid_get_double_value(solid_pop_stack(vm));
	double speed = solid_get_double_value(solid_pop_stack(vm));
	int health = solid_get_int_value(solid_pop_stack(vm));
	int h = solid_get_int_value(solid_pop_stack(vm));
	int w = solid_get_int_value(solid_pop_stack(vm));
	char *path = solid_get_str_value(solid_pop_stack(vm));
	entity *e = build_entity_prototype(path, w, h, health, speed, expval);
	vm->regs[255] = solid_struct(vm, e);
}

void __api_get_entity_x(solid_vm *vm)
{
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, ent->x);
}

void __api_get_entity_y(solid_vm *vm)
{
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, ent->y);
}

void __api_get_entity_w(solid_vm *vm)
{
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_int(vm, ent->w);
}

void __api_get_entity_h(solid_vm *vm)
{
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_int(vm, ent->h);
}

void __api_get_entity_xv(solid_vm *vm)
{
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, ent->xv);
}

void __api_get_entity_yv(solid_vm *vm)
{
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, ent->yv);
}

void __api_set_entity_xv(solid_vm *vm)
{
	double xv = solid_get_double_value(solid_pop_stack(vm));
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	ent->xv = xv;
}

void __api_set_entity_yv(solid_vm *vm)
{
	double yv = solid_get_double_value(solid_pop_stack(vm));
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	ent->yv = yv;
}

void __api_get_entity_speed(solid_vm *vm)
{
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, ent->speed);
}

void __api_set_entity_init(solid_vm *vm)
{
	solid_object *init = solid_pop_stack(vm);
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	set_entity_init(ent, init);
}

void __api_set_entity_hit(solid_vm *vm)
{
	solid_object *hit = solid_pop_stack(vm);
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	set_entity_hit(ent, hit);
}

void __api_set_entity_collide(solid_vm *vm)
{
	solid_object *collide = solid_pop_stack(vm);
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	set_entity_collide(ent, collide);
}

void __api_set_entity_update(solid_vm *vm)
{
	solid_object *update = solid_pop_stack(vm);
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	set_entity_update(ent, update);
}

void __api_get_entity_texture(solid_vm *vm)
{
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_struct(vm, ent->t);
}

void __api_make_entity(solid_vm *vm)
{
	double y = solid_get_double_value(solid_pop_stack(vm));
	double x = solid_get_double_value(solid_pop_stack(vm));
	char *t = solid_get_str_value(solid_pop_stack(vm));
	entity *e = make_entity(t, x, y);
	vm->regs[255] = solid_struct(vm, e);
}

void __api_spawn_entity(solid_vm *vm)
{
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	spawn_entity(ent);
}

void __api_give_entity_item(solid_vm *vm)
{
	item *weap = solid_get_struct_value(solid_pop_stack(vm));
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	give_entity_item(ent, weap);
}

void __api_move_entity(solid_vm *vm)
{
	int d = solid_get_int_value(solid_pop_stack(vm));
	entity *ent = solid_get_struct_value(solid_pop_stack(vm));
	move_entity(ent, d);
}

void initialize_entity()
{
	ENTITIES = make_list();

	ENTITY_PROTOTYPES = make_hash_map();

	defun("build_entity_prototype", __api_build_entity_prototype);
	defun("get_entity_x", __api_get_entity_x);
	defun("get_entity_y", __api_get_entity_y);
	defun("get_entity_w", __api_get_entity_w);
	defun("get_entity_h", __api_get_entity_h);
	defun("get_entity_xv", __api_get_entity_xv);
	defun("get_entity_yv", __api_get_entity_yv);
	defun("set_entity_xv", __api_set_entity_xv);
	defun("set_entity_yv", __api_set_entity_yv);
	defun("get_entity_speed", __api_get_entity_speed);
	defun("set_entity_init", __api_set_entity_init);
	defun("set_entity_hit", __api_set_entity_hit);
	defun("set_entity_collide", __api_set_entity_collide);
	defun("set_entity_update", __api_set_entity_update);
	defun("get_entity_texture", __api_get_entity_texture);
	defun("make_entity", __api_make_entity);
	defun("spawn_entity", __api_spawn_entity);
	defun("give_entity_item", __api_give_entity_item);
	defun("move_entity", __api_move_entity);

	load_all("script/entities/helpers");
	load_all("script/entities");
}

list_node *get_entities()
{
	return ENTITIES;
}

void reset_entity()
{
	list_node *c;
	for (c = ENTITIES->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((entity *) c->data) != NULL) {
			free_entity((entity *) c->data);
		}
	}
	ENTITIES = make_list();
}

void free_entity(entity *e)
{
	if (e != NULL) {
		free_item(e->item);
		free_texture(e->t);
		free(e);
	}
}

entity *build_entity_prototype(char *name, int w, int h,
		int health, double speed, double expval)
{
	entity *e = malloc(sizeof(entity));
	e->x = 0;
	e->y = 0;
	e->w = w;
	e->h = h;
	e->xv = e->yv = 0;
	e->t = NULL;
	e->item = NULL;
	e->health = health;
	e->speed = speed;
	e->expval = expval;
	e->init_func = e->hit_func = e->collide_func = e->update_func = NULL;
	e->data = solid_int(get_vm(), 0);
	set_hash(ENTITY_PROTOTYPES, name, (void *) e);
	return e;
}

void set_entity_init(entity *e, solid_object *init)
{
	e->init_func = init;
}

void set_entity_hit(entity *e, solid_object *hit)
{
	e->hit_func = hit;
}

void set_entity_collide(entity *e, solid_object *collide)
{
	e->collide_func = collide;
}

void set_entity_update(entity *e, solid_object *update)
{
	e->update_func = update;
}

entity *make_entity(char *name, double x, double y)
{
	entity *proto = (entity *) get_hash(ENTITY_PROTOTYPES, name);
	if (proto == NULL) {
		log_err("Entity prototype \"%s\" does not exist", name);
		exit(1);
	}
	entity *ret = malloc(sizeof(entity));
	memcpy(ret, proto, sizeof(entity));
	char buf[256] = "textures/entities/";
	strncat(buf, name, sizeof(buf) - strlen(buf) - 5);
	strcat(buf, ".png");
	if (!(ret->t = dungeon_load_texture(buf, 8, 8))) {
		if (!(ret->t = load_texture(buf, 8, 8))) {
			log_err("No texture found for entity \"%s\"", name);
			exit(1);
		}
	}
	ret->x = x;
	ret->y = y;
	if (ret->init_func != NULL) {
		solid_push_stack(get_vm(), solid_struct(get_vm(), ret));
		solid_call_func(get_vm(), ret->init_func);
		ret->data = get_vm()->regs[255];
	}
	return ret;
}

void spawn_entity(entity *e)
{
	insert_list(ENTITIES, (void *) e);
}

void hit_entity(entity *e, int dmg)
{
	if (e->hit_func != NULL) {
		solid_push_stack(get_vm(), e->data);
		solid_push_stack(get_vm(), solid_int(get_vm(), dmg));
		solid_push_stack(get_vm(), solid_struct(get_vm(), e));
		solid_call_func(get_vm(), e->hit_func);
		e->data = get_vm()->regs[255];
	} else {
		e->health -= dmg;
		if (e->health <= 0) {
			remove_list(ENTITIES, (void *) e);
			give_player_exp(e->expval);
			spawn_fx(make_fx(EXPLOSION, COLOR_WHITE, e->x, e->y, 4, 100, 20));
			free_entity(e);
		}
	}
}

void collide_entity(entity *e)
{
	if (e->collide_func != NULL) {
		solid_push_stack(get_vm(), e->data);
		solid_push_stack(get_vm(), solid_struct(get_vm(), e));
		solid_call_func(get_vm(), e->collide_func);
		e->data = get_vm()->regs[255];
	}
}

void give_entity_item(entity *e, item *i)
{
	i->x = &(e->x);
	i->y = &(e->y);
	e->item = i;
}

void move_entity(entity *e, direction d)
{
	switch (d) {
		case DIR_NORTH:
			e->xv = 0;
			e->yv = -e->speed;
			break;
		case DIR_SOUTH:
			e->xv = 0;
			e->yv = e->speed;
			break;
		case DIR_WEST:
			e->xv = -e->speed;
			e->yv = 0;
			break;
		case DIR_EAST:
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
	b.w = get_player_stat("width");
	b.h = get_player_stat("height");

	double tempx = e->x + e->xv;
	double tempy = e->y + e->yv;

	int shouldmovex = 1;
	int shouldmovey = 1;
	int curmovex = 0;
	int curmovey = 0;

	a.x = tempx;
	a.y = e->y;
	curmovex = check_collision(a, b);
	shouldmovex = shouldmovex ? curmovex : 0;
	a.x = e->x;
	a.y = tempy;
	curmovey = check_collision(a, b);
	shouldmovey = shouldmovey ? curmovey : 0;
	a.y = e->y;
	
	if (!shouldmovex || !shouldmovey) {
		collide_entity(e);
	}

	int xmin = (e->x/TILE_DIM)-2;
	int ymin = (e->y/TILE_DIM)-2;
	int xmax = (e->x/TILE_DIM)+2;
	int ymax = (e->y/TILE_DIM)+2;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_tile(x, y)) {
				/*b.x = x*TILE_DIM;
				b.y = y*TILE_DIM;
				b.w = b.h = TILE_DIM;
				a.x = tempx;
				a.y = e->y;
				shouldmovex = check_collision(a, b);
				a.x = e->x;
				a.y = tempy;
				shouldmovey = check_collision(a, b);
				a.y = e->y;*/
				b.x = x*TILE_DIM;
				b.y = y*TILE_DIM;
				b.w = b.h = TILE_DIM;
				a.x = tempx;
				a.y = e->y;
				curmovex = check_collision(a, b);
				shouldmovex = shouldmovex ? curmovex : 0;
				a.x = e->x;
				a.y = tempy;
				curmovey = check_collision(a, b);
				shouldmovey = shouldmovey ? curmovey : 0;
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
			if (e->update_func != NULL) {
				solid_push_stack(get_vm(), e->data);
				solid_push_stack(get_vm(), solid_struct(get_vm(), e));
				solid_call_func(get_vm(), e->update_func);
				e->data = get_vm()->regs[255];
			}
			check_entity_collisions(e);
		}
	}
}

void draw_entity()
{
	entity *e;
	list_node *c;
	for (c = ENTITIES->next; c->next != NULL; c = c->next) {
		if (((entity *) c->data) != NULL) {
			e = (entity *) c->data;
			draw_texture_scale(e->t, e->x, e->y, e->w, e->h);
		}
	}
}
