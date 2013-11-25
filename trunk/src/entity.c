#include "entity.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

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

list_node *get_entities()
{
	return ENTITIES;
}

void set_entities(list_node *entities)
{
	ENTITIES = entities;
}

void initialize_entity()
{
	ENTITIES = make_list();
	ENTITY_RESOURCES[0] = load_resource("textures/entities/slime.png");
	ENTITY_RESOURCES[1] = load_resource("textures/entities/wizard.png");
	ENTITY_RESOURCES[2] = load_resource("textures/entities/bossblob.png");
	ENTITY_RESOURCES[3] = load_resource("textures/entities/target.png");
}

void reset_entities()
{
	list_node *c;
	for (c = ENTITIES->next; c->next != NULL; c = c->next) {
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
	e->freeable = 1;
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
		spawn_fx(make_fx(EXPLOSION, e->weapon->c,
				e->x, e->y, 4, 100, 20));
		if (e->freeable == 1) {
			free(e);
		}
	}
}

void collide_entity(entity *e)
{
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
