#include "entity.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "resources.h"
#include "level.h"
#include "fx.h"
#include "player.h"
#include "weapon.h"

resource *ENTITY_RESOURCES[256];
list_node *ENTITIES;

static scm_t_bits __api_entity_tag;

SCM __api_make_entity(SCM id, SCM x, SCM y, SCM w, SCM h,
		SCM health, SCM speed, SCM expval)
{
	entity *e = make_entity(scm_to_int(id), scm_to_int(x), scm_to_int(y), scm_to_int(w), scm_to_int(h),
			scm_to_int(health), scm_to_int(speed), scm_to_double(expval));
	return scm_new_smob(__api_entity_tag, (unsigned long) e);
}

SCM __api_spawn_entity(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	spawn_entity(ent);
	return SCM_BOOL_T;
}

SCM __api_give_entity_weapon(SCM e, SCM w)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	give_entity_weapon(ent, weap);
	return SCM_BOOL_T;
}

SCM __api_move_entity(SCM e, SCM d)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	move_entity(ent, scm_to_int(d));
	return SCM_BOOL_T;
}

void initialize_entity()
{
	ENTITIES = make_list();
	ENTITY_RESOURCES[0] = load_resource("textures/entities/slime.png");
	ENTITY_RESOURCES[1] = load_resource("textures/entities/wizard.png");
	ENTITY_RESOURCES[2] = load_resource("textures/entities/bossblob.png");
	ENTITY_RESOURCES[3] = load_resource("textures/entities/target.png");

	__api_entity_tag = scm_make_smob_type("entity", sizeof(entity));
	scm_c_define_gsubr("make-entity", 8, 0, 0, __api_make_entity);
	scm_c_define_gsubr("spawn-entity", 1, 0, 0, __api_spawn_entity);
	scm_c_define_gsubr("give-entity-weapon", 2, 0, 0, __api_give_entity_weapon);
	scm_c_define_gsubr("move-entity", 2, 0, 0, __api_move_entity);
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
			scm_gc_free((entity *) c->data, sizeof(entity), "entity");
		}
	}
	ENTITIES = make_list();
}

entity *make_entity(int id, int x, int y, int w, int h,
		int health, int speed, double expval)
{
	entity *e = (entity *) scm_gc_malloc(sizeof(entity), "entity");
	e->id = id;
	e->x = x;
	e->y = y;
	e->w = w;
	e->h = h;
	e->xv = e->yv = 0;
	if (e->weapon != NULL) {
		e->weapon->x = &(e->x);
		e->weapon->y = &(e->y);
	}
	e->health = health;
	e->speed = speed;
	e->expval = expval;
	return e;
}

void spawn_entity(entity *e)
{
	insert_list(ENTITIES, (void *) e);
}

void hit_entity(entity *e, int dmg)
{
	e->health -= dmg;
	if (e->health <= 0) {
		remove_list(ENTITIES, (void *) e);
		give_player_exp(e->expval);
		spawn_fx(make_fx(EXPLOSION, COLOR_WHITE,
					e->x, e->y, 4, 100, 20));
		scm_gc_free(e, sizeof(entity), "entity");
	}
}

void collide_entity(entity *e)
{
}

void give_entity_weapon(entity *e, weapon *w)
{
	w->x = &(e->x);
	w->y = &(e->y);
	e->weapon = w;
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

	int xmin = (e->x/TILE_DIM)-2;
	int ymin = (e->y/TILE_DIM)-2;
	int xmax = (e->x/TILE_DIM)+2;
	int ymax = (e->y/TILE_DIM)+2;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_tile(x, y)) {
				b.x = x*32;
				b.y = y*32;
				b.w = b.h = TILE_DIM;
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
