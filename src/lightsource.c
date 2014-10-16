#include "lightsource.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <GL/glew.h>
#include <solid/solid.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "texture.h"
#include "level.h"

static list_node *LIGHTS;

static texture *LIGHTMAP;

void __api_make_lightsource(solid_vm *vm)
{
	color *col = solid_get_struct_value(solid_pop_stack(vm));
	int intensity = solid_get_int_value(solid_pop_stack(vm));
	int dim = solid_get_int_value(solid_pop_stack(vm));
	double y = solid_get_double_value(solid_pop_stack(vm));
	double x = solid_get_double_value(solid_pop_stack(vm));
	lightsource *l = make_lightsource(x, y, dim, intensity, *col);
	vm->regs[255] = solid_struct(vm, l);
}

void __api_spawn_lightsource(solid_vm *vm)
{
	spawn_lightsource(solid_get_struct_value(solid_pop_stack(vm)));
}

void initialize_lightsource()
{
	LIGHTS = make_list();
	LIGHTMAP = load_texture("textures/lightmap.png", 0, 0);

	defun("make_lightsource", __api_make_lightsource);
	defun("spawn_lightsource", __api_spawn_lightsource);
}

void reset_lightsource()
{
	list_node *c;
	for (c = LIGHTS->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((lightsource *) c->data) != NULL) {
			free((lightsource *) c->data);
		}
	}
	LIGHTS = make_list();
}

lightsource *make_lightsource(double x, double y, int dim, int intensity, color c)
{
	lightsource *l = malloc(sizeof(lightsource));
	l->x = x - dim/2;
	l->y = y - dim/2;
	l->dim = dim;
	l->intensity = intensity;
	l->c = c;
	return l;
}

void spawn_lightsource(lightsource *l)
{
	insert_list(LIGHTS, (void *) l);
}

void draw_lightsource()
{
	int w, h;
	w = LEVEL_MAX_DIM * TILE_DIM + 128;
	h = LEVEL_MAX_DIM * TILE_DIM + 128;

	glPushMatrix();
	glTranslatef(-64, -64, 0);
	glScalef(w, h, 1);
	glColor4f(0, 0, 0, get_current_level()->ambience);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, get_standard_vertices_handler());
	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();

	glBlendFunc(GL_DST_COLOR, GL_SRC_ALPHA);
	list_node *c;
	lightsource *l;
	int numtimes;
	int xdim, ydim;
	for (c = LIGHTS->next; c->next != NULL; c = c->next) {
		if (((lightsource *) c->data) != NULL) {
			l = (lightsource *) c->data;
			glPushMatrix();
			glTranslatef(l->x, l->y, 0);
			xdim = l->dim + (rand() % 16);
			ydim = l->dim + (rand() % 16);
			glScalef(xdim, ydim, 0);
			glColor3f(l->c.r, l->c.g, l->c.b);
			glBindTexture(GL_TEXTURE_2D, LIGHTMAP->texture);
			for (numtimes = l->intensity; numtimes > 0; numtimes--) {
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				glBindBuffer(GL_ARRAY_BUFFER, get_standard_vertices_handler());
				glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
				glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
				glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
			}
			glPopMatrix();
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
