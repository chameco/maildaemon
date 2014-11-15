#include "fx.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <solid/solid.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"

static list_node *PARTICLES;

static list_node *EFFECTS;
static hash_map *EFFECT_PROTOTYPES;

static list_node *GLOBAL_EFFECTS;

void __api_build_fx_prototype(solid_vm *vm) {
	solid_object *update = solid_pop_stack(vm);
	int lifespan = solid_get_int_value(solid_pop_stack(vm));
	char *name = solid_get_str_value(solid_pop_stack(vm));
	effect *ret = build_fx_prototype(name, lifespan, update);
	vm->regs[255] = solid_struct(vm, (void *) ret);
}

void __api_make_fx(solid_vm *vm)
{
	double y = solid_get_double_value(solid_pop_stack(vm));
	double x = solid_get_double_value(solid_pop_stack(vm));
	char *name = solid_get_str_value(solid_pop_stack(vm));
	effect *ret = make_fx(name, x, y);
	vm->regs[255] = solid_struct(vm, (void *) ret);
}

void __api_spawn_fx(solid_vm *vm)
{
	effect *e = (effect *) solid_get_struct_value(solid_pop_stack(vm));
	spawn_fx(e);
}

void __api_spawn_particle(solid_vm *vm)
{
	solid_object *update = solid_pop_stack(vm);
	int lifespan = solid_get_int_value(solid_pop_stack(vm));
	double h = solid_get_double_value(solid_pop_stack(vm));
	double w = solid_get_double_value(solid_pop_stack(vm));
	double y = solid_get_double_value(solid_pop_stack(vm));
	double x = solid_get_double_value(solid_pop_stack(vm));
	color c = *((color *) solid_get_struct_value(solid_pop_stack(vm)));
	spawn_particle(c, x, y, w, h, lifespan, update);
}

void __api_get_effect_x(solid_vm *vm)
{
	effect *e = (effect *) solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, e->x);
}

void __api_get_effect_y(solid_vm *vm)
{
	effect *e = (effect *) solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, e->y);
}

void __api_set_particle_color(solid_vm *vm)
{
	color *c = (color *) solid_get_struct_value(solid_pop_stack(vm));
	particle *p = (particle *) solid_get_struct_value(solid_pop_stack(vm));
	p->c = *c;
}

void __api_get_particle_x(solid_vm *vm)
{
	particle *p = (particle *) solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, p->x);
}

void __api_set_particle_x(solid_vm *vm)
{
	double x = solid_get_double_value(solid_pop_stack(vm));
	particle *p = (particle *) solid_get_struct_value(solid_pop_stack(vm));
	p->x = x;
}

void __api_get_particle_y(solid_vm *vm)
{
	particle *p = (particle *) solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, p->y);
}

void __api_set_particle_y(solid_vm *vm)
{
	double y = solid_get_double_value(solid_pop_stack(vm));
	particle *p = (particle *) solid_get_struct_value(solid_pop_stack(vm));
	p->y = y;
}

void __api_get_particle_w(solid_vm *vm)
{
	particle *p = (particle *) solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, p->w);
}

void __api_set_particle_w(solid_vm *vm)
{
	double w = solid_get_double_value(solid_pop_stack(vm));
	particle *p = (particle *) solid_get_struct_value(solid_pop_stack(vm));
	p->w = w;
}

void __api_get_particle_h(solid_vm *vm)
{
	particle *p = (particle *) solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, p->h);
}

void __api_set_particle_h(solid_vm *vm)
{
	double h = solid_get_double_value(solid_pop_stack(vm));
	particle *p = (particle *) solid_get_struct_value(solid_pop_stack(vm));
	p->h = h;
}

void initialize_fx()
{
	PARTICLES = make_list();

	EFFECTS = make_list();
	EFFECT_PROTOTYPES = make_hash_map();
	GLOBAL_EFFECTS = make_list();

	defun("build_fx_prototype", __api_build_fx_prototype);
	defun("make_fx", __api_make_fx);
	defun("spawn_fx", __api_spawn_fx);
	defun("spawn_particle", __api_spawn_particle);
	defun("set_particle_color", __api_set_particle_color);
	defun("get_effect_x", __api_get_effect_x);
	defun("get_effect_y", __api_get_effect_y);
	defun("get_particle_x", __api_get_particle_x);
	defun("set_particle_x", __api_set_particle_x);
	defun("get_particle_y", __api_get_particle_y);
	defun("set_particle_y", __api_set_particle_y);
	defun("get_particle_w", __api_get_particle_w);
	defun("set_particle_w", __api_set_particle_w);
	defun("get_particle_h", __api_get_particle_h);
	defun("set_particle_h", __api_set_particle_h);

	load_all("script/fx");
}

void reset_fx()
{
	list_node *c;
	for (c = PARTICLES->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((particle *) c->data) != NULL) {
			free((particle *) c->data);
		}
	}
	for (c = EFFECTS->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((effect *) c->data) != NULL) {
			free((effect *) c->data);
		}
	}
	for (c = GLOBAL_EFFECTS->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((global_effect *) c->data) != NULL) {
			free((global_effect *) c->data);
		}
	}
	PARTICLES = make_list();
	EFFECTS = make_list();
	GLOBAL_EFFECTS = make_list();
}

effect *build_fx_prototype(char *name, int lifespan, solid_object *update)
{
	effect *e = (effect *) malloc(sizeof(effect));
	e->x = e->y = 0;
	e->lifespan = lifespan;
	e->update = update;
	solid_mark_object(e->update, 2);
	set_hash(EFFECT_PROTOTYPES, name, e);
	return e;
}

effect *make_fx(char *name, double x, double y)
{
	effect *proto = (effect *) get_hash(EFFECT_PROTOTYPES, name);
	if (proto == NULL) {
		log_err("Effect prototype \"%s\" does not exist", name);
		exit(1);
	}
	effect *ret = malloc(sizeof(effect));
	memcpy(ret, proto, sizeof(effect));
	ret->x = x;
	ret->y = y;
	return ret;
}

void spawn_fx(effect *e)
{
	insert_list(EFFECTS, (void *) e);
}

void spawn_particle(color c, double x, double y, double w, double h, int lifespan, solid_object *update)
{
	particle *p = (particle *) malloc(sizeof(particle));
	p->c = c;
	p->x = x;
	p->y = y;
	p->w = w;
	p->h = h;
	p->lifespan = lifespan;
	p->update = update;
	insert_list(PARTICLES, (void *) p);
}

global_effect *make_global_fx(void (*callback)(), int lifespan)
{
	global_effect *ge = malloc(sizeof(global_effect));
	ge->callback = callback;
	ge->lifespan = lifespan;
	return ge;
}

void global_effect_shake()
{
	glTranslatef((rand() % 6) - 3, (rand() % 6) - 3, 0);
}

void spawn_global_fx(global_effect *e)
{
	insert_list(GLOBAL_EFFECTS, (void *) e);
}

void update_fx()
{
	list_node *c;
	global_effect *ge;
	for (c = GLOBAL_EFFECTS->next; c->next != NULL; c = c->next) {
		ge = (global_effect *) c->data;
		if (ge != NULL) {
			if (ge->lifespan > 0) {
				ge->lifespan -= 1;
			}
			if (ge->lifespan == 0) {
				c->prev->next = c->next;
				c->next->prev = c->prev;
				free(c);
				free(ge);
			}
		}
	}
	effect *e;
	for (c = EFFECTS->next; c->next != NULL; c = c->next) {
		e = (effect *) c->data;
		if (e != NULL) {
			solid_push_stack(get_vm(), solid_struct(get_vm(), (void *) e));
			solid_call_func(get_vm(), e->update);
			if (e->lifespan > 0) {
				e->lifespan -= 1;
			}
			if (e->lifespan == 0) {
				c->prev->next = c->next;
				c->next->prev = c->prev;
				free(c);
				free(e);
			}
		}
	}
	particle *p;
	for (c = PARTICLES->next; c->next != NULL; c = c->next) {
		p = (particle *) c->data;
		if (p != NULL) {
			solid_push_stack(get_vm(), solid_struct(get_vm(), (void *) p));
			solid_call_func(get_vm(), p->update);
			if (p->lifespan > 0) {
				p->lifespan -= 1;
			}
			if (p->lifespan == 0) {
				c->prev->next = c->next;
				c->next->prev = c->prev;
				free(c);
				free(p);
			}
		}
	}
}

void apply_global_fx()
{
	list_node *c;
	for (c = GLOBAL_EFFECTS->next; c->next != NULL; c = c->next) {
		if (c->data != NULL) {
			((global_effect *) c->data)->callback();
		}
	}
}

void draw_particle(particle *p)
{
	glPushMatrix();
	glTranslatef(p->x, p->y, 0);
	glScalef(p->w, p->h, 1);

	glColor3f(p->c.r, p->c.g, p->c.b);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, get_standard_vertices_handler());

	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0x0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void draw_fx()
{
	list_node *c;
	for (c = PARTICLES->next; c->next != NULL; c = c->next) {
		if (((particle *) c->data) != NULL) {
			draw_particle((particle *) c->data);
		}
	}
}
