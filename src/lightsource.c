#include "lightsource.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "texture.h"
#include "level.h"

static list_node *LIGHTS;

static texture *LIGHTMAP;

static scm_t_bits __api_lightsource_tag;

SCM __api_make_lightsource(SCM x, SCM y, SCM dim, SCM intensity, SCM c)
{
	color *col = (color *) SCM_SMOB_DATA(c);
	lightsource *l = make_lightsource(scm_to_int(x), scm_to_int(y), scm_to_int(dim), scm_to_int(intensity), *col);
	return scm_new_smob(__api_lightsource_tag, (unsigned long) l);
}

SCM __api_spawn_lightsource(SCM l)
{
	lightsource *light = (lightsource *) SCM_SMOB_DATA(l);
	spawn_lightsource(light);
	scm_gc_protect_object(l);
	return SCM_BOOL_F;
}

void initialize_lightsource()
{
	LIGHTS = make_list();
	LIGHTMAP = load_texture("textures/lightmap.png", 0, 0);

	__api_lightsource_tag = scm_make_smob_type("lightsource", sizeof(lightsource));
	scm_c_define_gsubr("make-lightsource", 5, 0, 0, __api_make_lightsource);
	scm_c_define_gsubr("spawn-lightsource", 1, 0, 0, __api_spawn_lightsource);
}

void reset_lightsource()
{
	list_node *c;
	for (c = LIGHTS->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((lightsource *) c->data) != NULL) {
			scm_gc_free((lightsource *) c->data, sizeof(lightsource), "lightsource");
		}
	}
	LIGHTS = make_list();
}

lightsource *make_lightsource(int x, int y, int dim, int intensity, color c)
{
	lightsource *l = scm_gc_malloc(sizeof(lightsource), "lightsource");
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
	//glBlendFunc(GL_DST_COLOR, GL_ZERO);
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
