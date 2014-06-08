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
#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"

static list_node *EFFECTS;
static list_node *GLOBAL_EFFECTS;
static vertex EFFECT_VERTICES[4];
static GLuint EFFECT_VERTEX_HANDLER = 0;
static Mix_Chunk *EXPLOSION_SOUND;

static scm_t_bits __api_effect_tag;

SCM __api_make_fx(SCM type, SCM col, SCM x, SCM y, SCM dim, SCM radius, SCM speed)
{
	color c = *((color *) SCM_SMOB_DATA(col));
	SCM ret = scm_new_smob(__api_effect_tag,
			(unsigned long) make_fx(scm_to_int(type), c,
				scm_to_int(x), scm_to_int(y), scm_to_int(dim),
				scm_to_int(radius), scm_to_int(speed)));
	scm_gc_protect_object(ret);
	return ret;
}

SCM __api_spawn_fx(SCM e)
{
	effect *fx = (effect *) SCM_SMOB_DATA(e);
	spawn_fx(fx);
	return SCM_BOOL_F;
}

void initialize_fx()
{
	EFFECTS = make_list();
	GLOBAL_EFFECTS = make_list();
	EXPLOSION_SOUND = Mix_LoadWAV("sfx/explode.wav");

	EFFECT_VERTICES[0].x = 0;
	EFFECT_VERTICES[0].y = 0;

	EFFECT_VERTICES[1].x = 1;
	EFFECT_VERTICES[1].y = 0;

	EFFECT_VERTICES[2].x = 1;
	EFFECT_VERTICES[2].y = 1;

	EFFECT_VERTICES[3].x = 0;
	EFFECT_VERTICES[3].y = 1;

	glGenBuffers(1, &EFFECT_VERTEX_HANDLER);
	glBindBuffer(GL_ARRAY_BUFFER, EFFECT_VERTEX_HANDLER);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), EFFECT_VERTICES, GL_STATIC_DRAW);

	__api_effect_tag = scm_make_smob_type("effect", sizeof(effect));
	scm_c_define_gsubr("make-fx", 7, 0, 0, __api_make_fx);
	scm_c_define_gsubr("spawn-fx", 1, 0, 0, __api_spawn_fx);
}

void reset_fx()
{
	list_node *c;
	for (c = EFFECTS->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((effect *) c->data) != NULL) {
			free((effect *) c->data);
		}
	}
	for (c = GLOBAL_EFFECTS->next; c->next != NULL; c = c->next, free(c->prev)) {}
	EFFECTS = make_list();
	GLOBAL_EFFECTS = make_list();
}

effect *make_fx(etype type, color col,
		int x, int y, int dim,
		int radius, int speed)
{
	effect *e = malloc(sizeof(effect));
	e->type = type;
	e->c = col; e->x = x;
	e->y = y;
	e->dim = dim;
	e->radius = radius;
	e->speed = speed;
	e->cur = 0;
	e->statelen = 30;
	int c;
	switch (e->type) {
		case EXPLOSION:
			for (c = 0; c < e->statelen; c++) {
				if (rand() % 2) {
					e->state[c][0] = rand() % e->radius/4;
				} else {
					e->state[c][0] = -(rand() % e->radius/4);
				}
				if (rand() % 2) {
					e->state[c][1] = rand() % e->radius/4;
				} else {
					e->state[c][1] = -(rand() % e->radius/4);
				}
				e->state[c][2] = e->state[c][0];
				e->state[c][3] = e->state[c][1];
				e->state[c][4] = 1;
				Mix_PlayChannel(-1, EXPLOSION_SOUND, 0);
			}
			break;
		case SMOKE_CONST:
		case SMOKE:
			for (c = 0; c < e->statelen; c++) {
				if (rand() % 2) {
					e->state[c][0] = rand() % e->radius/4;
				} else {
					e->state[c][0] = -(rand() % e->radius/4);
				}
				e->state[c][1] = -(rand() % e->radius);
				e->state[c][4] = 1;
			}
			break;
	}
	return e;
}

void spawn_fx(effect *e)
{
	insert_list(EFFECTS, (void *) e);
}

global_effect *make_global_fx(void (*callback)(), int timer)
{
	global_effect *ge = malloc(sizeof(global_effect));
	ge->callback = callback;
	ge->timer = timer;
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
	effect *e;
	global_effect *ge;
	int counter;
	for (c = GLOBAL_EFFECTS->next; c->next != NULL; c = c->next) {
		ge = (global_effect *) c->data;
		if (ge != NULL) {
			ge->timer -= 1;
			if (ge->timer <= 0) {
				c->prev->next = c->next;
				c->next->prev = c->prev;
				free(c);
			}
		}
	}
	for (c = EFFECTS->next; c->next != NULL; c = c->next) {
		e = (effect *) c->data;
		if (e != NULL) {
			switch (e->type) {
				case EXPLOSION:
					e->cur += e->speed;
					if (e->cur >= e->radius) {
						c->prev->next = c->next;
						c->next->prev = c->prev;
						free(c);
						free(e);
					} else {
						for (counter = 0; counter < e->statelen; counter++) {
							if (!(e->state[counter][2] == 0 && e->state[counter][3] == 0)) {
								if (e->state[counter][4]) {
									e->state[counter][0] += e->state[counter][2];
									e->state[counter][1] += e->state[counter][3];
									e->state[counter][2] /= 2;
									e->state[counter][3] /= 2;
								}
							}
						}
					}
					break;
				case SMOKE_CONST:
					for (counter = 0; counter < e->statelen; counter++) {
						if (rand() % 2) {
							e->state[counter][0] += rand() % 3;
						} else {
							e->state[counter][0] -= rand() % 3;
						}
						if (e->state[counter][1] >= -(e->radius)) {
							e->state[counter][1] -= rand() % 3;
						} else {
							e->state[counter][1] = 0;
							if (rand() % 2) {
								e->state[counter][0] = rand() % e->radius/4;
							} else {
								e->state[counter][0] = -(rand() % e->radius/4);
							}
						}
					}
					break;
				case SMOKE:
					e->cur += e->speed;
					if (e->cur >= 100) {
						c->prev->next = c->next;
						c->next->prev = c->prev;
						free(c);
						free(e);
					} else {
						for (counter = 0; counter < e->statelen; counter++) {
							if (e->state[counter][4]) {
								if (rand() % 2) {
									e->state[counter][0] += rand() % 3;
								} else {
									e->state[counter][0] -= rand() % 3;
								}
								if (e->state[counter][1] >= -(e->radius)) {
									e->state[counter][1] -= rand() % 5;
								} else {
									e->state[counter][2] = 0;
								}
							}
						}
					}
					break;
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

inline void draw_particle(effect *e, int xdiff, int ydiff)
{
	glPushMatrix();
	glTranslatef(e->x+xdiff, e->y+ydiff, 0);
	glScalef(e->dim, e->dim, 1);

	glColor4f(e->c.r, e->c.g, e->c.b, e->c.a);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, EFFECT_VERTEX_HANDLER);
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

inline void draw_smoke_particle(effect *e, int xdiff, int ydiff)
{
	GLfloat r, g, b;
	GLfloat factor;
	factor = (GLfloat) (((float) (rand() % 1000)) / 1000);
	r = e->c.r * factor;
	r = r > 1.0 ? 1.0 : r;
	r = r < 0.0 ? 0.0 : r;

	g = e->c.g * factor;
	g = g > 1.0 ? 1.0 : g;
	g = g < 0.0 ? 0.0 : g;
	
	b = e->c.b * factor;
	b = b > 1.0 ? 1.0 : b;
	b = b < 0.0 ? 0.0 : b;

	glPushMatrix();
	glTranslatef(e->x+xdiff, e->y+ydiff, 0);
	glScalef(e->dim, e->dim, 1);

	glColor4f(r, g, b, e->c.a);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, EFFECT_VERTEX_HANDLER);
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void draw_one_fx(effect *e)
{
	int c;
	switch (e->type) {
		case EXPLOSION:
			for (c = 0; c < e->statelen; c++) {
				if (e->state[c][4]) {
					draw_particle(e, e->state[c][0], e->state[c][1]);
				}
			}
			break;
		case SMOKE_CONST:
		case SMOKE:
			for (c = 0; c < e->statelen; c++) {
				if (e->state[c][4]) {
					draw_smoke_particle(e, e->state[c][0], e->state[c][1]);
				}
			}
	}
}

void draw_fx()
{
	list_node *c;
	for (c = EFFECTS->next; c->next != NULL; c = c->next) {
		if (((effect *) c->data) != NULL) {
			draw_one_fx((effect *) c->data);
		}
	}
}
