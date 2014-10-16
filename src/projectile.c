#include "projectile.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <solid/solid.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "item.h"
#include "player.h"
#include "entity.h"
#include "level.h"

static list_node *PROJECTILES;

static hash_map *PROJECTILE_PROTOTYPES;

void __api_build_projectile_prototype(solid_vm *vm)
{
	int damage = solid_get_int_value(solid_pop_stack(vm));
	int longevity = solid_get_int_value(solid_pop_stack(vm));
	int h = solid_get_int_value(solid_pop_stack(vm));
	int w = solid_get_int_value(solid_pop_stack(vm));
	double speed = solid_get_double_value(solid_pop_stack(vm));
	char *s = solid_get_str_value(solid_pop_stack(vm));
	projectile *p = build_projectile_prototype(s, speed, w, h, longevity, damage);
	vm->regs[255] = solid_struct(vm, p);
}

void __api_make_projectile(solid_vm *vm)
{
	item *sb = solid_get_struct_value(solid_pop_stack(vm));
	double rotation = solid_get_double_value(solid_pop_stack(vm));
	double y = solid_get_double_value(solid_pop_stack(vm));
	double x = solid_get_double_value(solid_pop_stack(vm));
	char *s = solid_get_str_value(solid_pop_stack(vm));
	projectile *p = make_projectile(s, x, y, rotation, sb);
	vm->regs[255] = solid_struct(vm, p);
}

void __api_spawn_projectile(solid_vm *vm)
{
	projectile *p = solid_get_struct_value(solid_pop_stack(vm));
	spawn_projectile(p);
}

void initialize_projectile()
{
	PROJECTILES = make_list();

	PROJECTILE_PROTOTYPES = make_hash_map();

	defun("build_projectile_prototype", __api_build_projectile_prototype);
	defun("make_projectile", __api_make_projectile);
	defun("spawn_projectile", __api_spawn_projectile);

	load_all("script/projectiles");
}

void reset_projectile()
{
	list_node *c;
	for (c = PROJECTILES->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((projectile *) c->data) != NULL) {
			free((projectile *) c->data);
		}
	}
	PROJECTILES = make_list();
}

projectile *build_projectile_prototype(char *name, double speed, int w, int h, int longevity, int dmg)
{
	projectile *p = malloc(sizeof(projectile));
	p->w = w;
	p->h = h;
	p->speed = speed;
	p->longevity = longevity;
	p->dmg = dmg;

	p->x = p->y = p->xv = p->yv = 0;
	p->rotation = 0;
	p->spawned_by = NULL;

	set_hash(PROJECTILE_PROTOTYPES, name, (void *) p);

	return p;
}

projectile *make_projectile(char *name, double x, double y, double rotation, item *spawned_by)
{
	projectile *proto = (projectile *) get_hash(PROJECTILE_PROTOTYPES, name);
	if (proto == NULL) {
		log_err("Projectile prototype \"%s\" does not exist", name);
		exit(1);
	}
	projectile *p = malloc(sizeof(projectile));
	memcpy(p, proto, sizeof(projectile));
	char buf[256] = "textures/projectiles/";
	strncat(buf, name, sizeof(buf) - strlen(buf) - 5);
	strcat(buf, ".png");
	p->t = load_texture(buf, p->w, p->h);
	p->x = x;
	p->y = y;
	p->rotation = rotation;
	p->xv = p->speed * cos(rotation);
	p->yv = p->speed * sin(rotation);
	p->spawned_by = spawned_by;

	return p;
}

void spawn_projectile(projectile *p)
{
	insert_list(PROJECTILES, (void *) p);
}

void check_projectile_collisions(projectile *p)
{
	SDL_Rect a = {
		p->x,
		p->y,
		p->w,
		p->h
	};
	SDL_Rect b;

	if (p->spawned_by != get_player_item()) {
		b.x = get_player_x();
		b.y = get_player_y();
		b.w = get_player_stat("width");
		b.h = get_player_stat("height");
		if (!check_collision(a, b)) {
			hit_player(p->dmg);
			destroy_projectile(p);
			return;
		}

	}

	int xmin = (p->x/TILE_DIM)-2;
	int ymin = (p->y/TILE_DIM)-2;
	int xmax = (p->x/TILE_DIM)+2;
	int ymax = (p->y/TILE_DIM)+2;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_tile(x, y)) {
				b.x = x*32;
				b.y = y*32;
				b.w = b.h = TILE_DIM;
				if (!check_collision(a, b)) {
					destroy_projectile(p);
					return;
				}
			}
		}
	}

	list_node *entities = get_entities();
	list_node *c;
	entity *e;
	for (c = entities->next; c->next != NULL; c = c->next) {
		if (((entity *) c->data) != NULL &&
				((entity *) c->data)->item != p->spawned_by) {
			e = (entity *) c->data;
			b.x = e->x;
			b.y = e->y;
			b.w = e->w;
			b.h = e->h;
			if (!check_collision(a, b)) {
				hit_entity(e, p->dmg);
				destroy_projectile(p);
				return;
			}
		}
	}
}

void destroy_projectile(projectile *p)
{
	remove_list(PROJECTILES, p);
}

void update_projectile()
{
	list_node *c;
	projectile *p;
	for (c = PROJECTILES->next; c->next != NULL; c = c->next) {
		if (((projectile *) c->data) != NULL) {
			p = (projectile *) c->data;
			p->x += p->xv;
			p->y += p->yv;
			if (--p->longevity <= 0) {
				destroy_projectile(p);
			}
			check_projectile_collisions(p);
		}
	}
}

void draw_projectile()
{
	list_node *c;
	projectile *p;
	for (c = PROJECTILES->next; c->next != NULL; c = c->next) {
		if (((projectile *) c->data) != NULL) {
			p = (projectile *) c->data;
			draw_texture_scale(p->t, p->x, p->y, p->w, p->h);
		}
	}
}
