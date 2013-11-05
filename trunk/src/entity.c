#include "entity.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <cuttle/debug.h>
#include <cuttle/utils.h>
#include "utils.h"
#include "resources.h"
#include "worldgen.h"
#include "level.h"
#include "fx.h"
#include "player.h"
#include "weapon.h"
#include "projectile.h"

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
			free((entity *) c->data);
		}
	}
	ENTITIES = make_list();
}

void spawn_entity(int id, int x, int y, int w, int h,
		weapon *weapon, int health, int speed, double expval)
{
	entity *e = (entity *) malloc(sizeof(entity));
	e->id = id;
	e->x = x;
	e->y = y;
	e->w = w;
	e->h = h;
	e->weapon = weapon;
	e->weapon->x = &(e->x);
	e->weapon->y = &(e->y);
	e->health = health;
	e->speed = speed;
	e->expval = expval;
	insert_list(ENTITIES, (void *) e);
}

void hit_entity(entity *e, int dmg)
{
	e->health -= dmg;
	if (e->health <= 0) {
		remove_list(ENTITIES, (void *) e);
		give_player_exp(e->expval);
		spawn_fx(EXPLOSION, e->weapon->c,
				e->x, e->y, 4, 100, 20);
	}
}

void collide_entity(entity *e)
{
}

void move_entity_north(entity *e)
{
	SDL_Rect a = {
		e->x,
		e->y - e->speed,
		e->w,
		e->h
	};
	move_entity_worker(e, a);
}

void move_entity_south(entity *e)
{
	SDL_Rect a = {
		e->x,
		e->y + e->speed,
		e->w,
		e->h
	};
	move_entity_worker(e, a);
}

void move_entity_west(entity *e)
{
	SDL_Rect a = {
		e->x - e->speed,
		e->y,
		e->w,
		e->h
	};
	move_entity_worker(e, a);
}

void move_entity_east(entity *e)
{
	SDL_Rect a = {
		e->x + e->speed,
		e->y,
		e->w,
		e->h
	};
	move_entity_worker(e, a);
}

void move_entity_worker(entity *e, SDL_Rect a)
{
	SDL_Rect b;

	b.x = get_player_x();
	b.y = get_player_y();
	b.w = get_player_w();
	b.h = get_player_h();
	if (!check_collision(a, b)) {
		collide_entity(e);
		return;
	}

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
				if (!check_collision(a, b)) {
					return;
				}
			}
		}
	}
	e->x = a.x;
	e->y = a.y;
}

void update_entity()
{
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
