#include "entity.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include <GL/glew.h>
#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "texture.h"
#include "level.h"
#include "fx.h"
#include "player.h"
#include "weapon.h"

static list_node *ENTITIES;

static hash_map *ENTITY_PROTOTYPES;

static scm_t_bits __api_entity_tag;

SCM __api_build_entity_prototype(SCM path, SCM w, SCM h,
		SCM health, SCM speed, SCM expval)
{
	entity *e = build_entity_prototype(scm_to_locale_string(path), scm_to_int(w), scm_to_int(h),
			scm_to_int(health), scm_to_int(speed), scm_to_double(expval));
	return scm_new_smob(__api_entity_tag, (unsigned long) e);
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

SCM __api_make_entity(SCM name, SCM x, SCM y)
{
	return scm_new_smob(__api_entity_tag, (unsigned long) make_entity(scm_to_locale_string(name), scm_to_int(x), scm_to_int(y)));
}

SCM __api_spawn_entity(SCM e)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	spawn_entity(ent);
	scm_gc_protect_object(e);
	return SCM_BOOL_F;
}

SCM __api_give_entity_weapon(SCM e, SCM w)
{
	entity *ent = (entity *) SCM_SMOB_DATA(e);
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	give_entity_weapon(ent, weap);
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
	scm_c_define_gsubr("set-entity-init", 2, 0, 0, __api_set_entity_init);
	scm_c_define_gsubr("set-entity-hit", 2, 0, 0, __api_set_entity_hit);
	scm_c_define_gsubr("set-entity-collide", 2, 0, 0, __api_set_entity_collide);
	scm_c_define_gsubr("set-entity-update", 2, 0, 0, __api_set_entity_update);
	scm_c_define_gsubr("make-entity", 3, 0, 0, __api_make_entity);
	scm_c_define_gsubr("spawn-entity", 1, 0, 0, __api_spawn_entity);
	scm_c_define_gsubr("give-entity-weapon", 2, 0, 0, __api_give_entity_weapon);
	scm_c_define_gsubr("move-entity", 2, 0, 0, __api_move_entity);

	DIR *d = opendir("entities");
	struct dirent *entry;
	char buf[256];
	if (d != NULL) {
		while ((entry = readdir(d))) {
			char *pos = strrchr(entry->d_name, '.') + 1;
			if (pos != NULL && strcmp(pos, "scm") == 0) {
				strcpy(buf, "entities/");
				strcat(buf, entry->d_name);
				scm_c_primitive_load(buf);
			}
		}
		closedir(d);
	} else {
		log_err("Directory \"entities\" does not exist");
	}
}

list_node *get_entities()
{
	return ENTITIES;
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

entity *build_entity_prototype(char *name, int w, int h,
		int health, int speed, double expval)
{
	entity *e = scm_gc_malloc(sizeof(entity), "entity");
	strcpy(e->name, name);
	e->x = 0;
	e->y = 0;
	e->w = w;
	e->h = h;
	e->xv = e->yv = 0;
	e->r = NULL;
	e->weapon = NULL;
	e->health = health;
	e->speed = speed;
	e->expval = expval;
	e->hit_func = e->collide_func = e->update_func = SCM_BOOL_F;
	set_hash(ENTITY_PROTOTYPES, name, e);
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

entity *make_entity(char *name, int x, int y)
{
	entity *proto = get_hash(ENTITY_PROTOTYPES, name);
	entity *ret = scm_gc_malloc(sizeof(entity), "entity");
	memcpy(ret, proto, sizeof(entity));
	char buf[256] = "textures/entities/";
	strncat(buf, name, sizeof(buf) - strlen(buf) - 5);
	strcat(buf, ".png");
	ret->r = load_texture(buf, 8, 8);
	ret->x = x;
	ret->y = y;
	return ret;
}

void spawn_entity(entity *e)
{
	insert_list(ENTITIES, (void *) e);
}

void hit_entity(entity *e, int dmg)
{
	if (scm_is_true(e->hit_func)) {
		scm_call_2(e->hit_func, scm_new_smob(__api_entity_tag, (unsigned long) e), scm_from_int(dmg));
	} else {
		e->health -= dmg;
		if (e->health <= 0) {
			remove_list(ENTITIES, (void *) e);
			give_player_exp(e->expval);
			spawn_fx(make_fx(EXPLOSION, COLOR_WHITE,
						e->x, e->y, 4, 100, 20));
			scm_gc_free(e, sizeof(entity), "entity");
		}
	}
}

void collide_entity(entity *e)
{
	if (scm_is_true(e->collide_func)) {
		scm_call_1(e->collide_func, scm_new_smob(__api_entity_tag, (unsigned long) e));
	}
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
			if (scm_is_true(e->collide_func)) {
				scm_call_1(e->collide_func, scm_new_smob(__api_entity_tag, (unsigned long) e));
			}
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
			draw_texture_scale(e->r, e->x, e->y, e->w, e->h);
		}
	}
}
