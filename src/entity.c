#include "entity.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include <GL/glew.h>
#include <libguile.h>

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

static scm_t_bits __api_entity_tag;

SCM __api_build_entity_prototype(SCM path, SCM w, SCM h,
		SCM health, SCM speed, SCM expval)
{
	char *t = scm_to_locale_string(path);
	entity *e = build_entity_prototype(t, scm_to_int(w), scm_to_int(h),
			scm_to_int(health), scm_to_double(speed), scm_to_double(expval));
	free(t);
	SCM ret = scm_new_smob(__api_entity_tag, (unsigned long) e);
	scm_gc_protect_object(ret);
	return ret;
}

SCM __api_get_entity_x(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	return scm_from_double(ent->x);
}

SCM __api_get_entity_y(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	return scm_from_double(ent->y);
}

SCM __api_get_entity_w(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	return scm_from_int(ent->w);
}

SCM __api_get_entity_h(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	return scm_from_int(ent->h);
}

SCM __api_get_entity_xv(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	return scm_from_double(ent->xv);
}

SCM __api_get_entity_yv(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	return scm_from_double(ent->yv);
}

SCM __api_set_entity_xv(SCM e, SCM xv)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	ent->xv = scm_to_double(xv);
	return SCM_BOOL_F;
}

SCM __api_set_entity_yv(SCM e, SCM yv)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	ent->yv = scm_to_double(yv);
	return SCM_BOOL_F;
}

SCM __api_get_entity_speed(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	return scm_from_double(ent->speed);
}

SCM __api_set_entity_init(SCM e, SCM init)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	set_entity_init(ent, init);
	return SCM_BOOL_F;
}

SCM __api_set_entity_hit(SCM e, SCM hit)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	set_entity_hit(ent, hit);
	return SCM_BOOL_F;
}

SCM __api_set_entity_collide(SCM e, SCM collide)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	set_entity_collide(ent, collide);
	return SCM_BOOL_F;
}

SCM __api_set_entity_update(SCM e, SCM update)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	set_entity_update(ent, update);
	return SCM_BOOL_F;
}

SCM __api_get_entity_texture(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	return scm_new_smob(get_texture_tag(), (unsigned long) ent->t);
}

SCM __api_make_entity(SCM name, SCM x, SCM y)
{
	char *t = scm_to_locale_string(name);
	entity *e = make_entity(t, scm_to_double(x), scm_to_double(y));
	free(t);
	SCM ret = scm_new_smob(__api_entity_tag, (unsigned long) e);
	scm_gc_protect_object(ret);
	return ret;
}

SCM __api_spawn_entity(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	spawn_entity(ent);
	return SCM_BOOL_F;
}

SCM __api_give_entity_item(SCM e, SCM w)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	item *weap = (item *) SCM_SMOB_DATA(w);
	give_entity_item(ent, weap);
	return SCM_BOOL_F;
}

SCM __api_move_entity(SCM e, SCM d)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	move_entity(ent, scm_to_int(d));
	return SCM_BOOL_F;
}

SCM __api_smob_entity_mark(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	scm_gc_mark(ent->init_func);
	scm_gc_mark(ent->hit_func);
	scm_gc_mark(ent->collide_func);
	return ent->update_func;
}

void initialize_entity()
{
	ENTITIES = make_list();

	ENTITY_PROTOTYPES = make_hash_map();

	__api_entity_tag = scm_make_smob_type("entity", sizeof(entity));
	scm_set_smob_mark(__api_entity_tag, __api_smob_entity_mark);
	scm_c_define_gsubr("build-entity-prototype", 6, 0, 0, __api_build_entity_prototype);
	scm_c_define_gsubr("get-entity-x", 1, 0, 0, __api_get_entity_x);
	scm_c_define_gsubr("get-entity-y", 1, 0, 0, __api_get_entity_y);
	scm_c_define_gsubr("get-entity-w", 1, 0, 0, __api_get_entity_w);
	scm_c_define_gsubr("get-entity-h", 1, 0, 0, __api_get_entity_h);
	scm_c_define_gsubr("get-entity-xv", 1, 0, 0, __api_get_entity_xv);
	scm_c_define_gsubr("get-entity-yv", 1, 0, 0, __api_get_entity_yv);
	scm_c_define_gsubr("set-entity-xv", 2, 0, 0, __api_set_entity_xv);
	scm_c_define_gsubr("set-entity-yv", 2, 0, 0, __api_set_entity_yv);
	scm_c_define_gsubr("get-entity-speed", 1, 0, 0, __api_get_entity_speed);
	scm_c_define_gsubr("set-entity-init", 2, 0, 0, __api_set_entity_init);
	scm_c_define_gsubr("set-entity-hit", 2, 0, 0, __api_set_entity_hit);
	scm_c_define_gsubr("set-entity-collide", 2, 0, 0, __api_set_entity_collide);
	scm_c_define_gsubr("set-entity-update", 2, 0, 0, __api_set_entity_update);
	scm_c_define_gsubr("get-entity-texture", 1, 0, 0, __api_get_entity_texture);
	scm_c_define_gsubr("make-entity", 3, 0, 0, __api_make_entity);
	scm_c_define_gsubr("spawn-entity", 1, 0, 0, __api_spawn_entity);
	scm_c_define_gsubr("give-entity-item", 2, 0, 0, __api_give_entity_item);
	scm_c_define_gsubr("move-entity", 2, 0, 0, __api_move_entity);

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
	e->data = e->init_func = e->hit_func = e->collide_func = e->update_func = SCM_BOOL_F;
	set_hash(ENTITY_PROTOTYPES, name, (void *) e);
	return e;
}

void set_entity_init(entity *e, SCM init)
{
	e->init_func = init;
}

void set_entity_hit(entity *e, SCM hit)
{
	e->hit_func = hit;
}

void set_entity_collide(entity *e, SCM collide)
{
	e->collide_func = collide;
}

void set_entity_update(entity *e, SCM update)
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
	if (scm_is_true(ret->init_func)) {
		ret->data = scm_call_1(ret->init_func, scm_new_smob(__api_entity_tag, (unsigned long) ret));
	}
	return ret;
}

void spawn_entity(entity *e)
{
	insert_list(ENTITIES, (void *) e);
}

void hit_entity(entity *e, int dmg)
{
	if (scm_is_true(e->hit_func)) {
		e->data = scm_call_3(e->hit_func, scm_new_smob(__api_entity_tag, (unsigned long) e), scm_from_int(dmg), e->data);
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
	if (scm_is_true(e->collide_func)) {
		e->data = scm_call_2(e->collide_func, scm_new_smob(__api_entity_tag, (unsigned long) e), e->data);
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
			if (scm_is_true(e->update_func)) {
				e->data = scm_call_2(e->update_func, scm_new_smob(__api_entity_tag, (unsigned long) e), e->data);
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
