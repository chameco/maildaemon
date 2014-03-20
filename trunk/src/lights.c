#include "lights.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "vm.h"
#include "utils.h"
#include "resources.h"
#include "level.h"

list_node *LIGHTS;

resource *LIGHTMAP;
GLuint LIGHT_OVERLAY;

void initialize_lights()
{
	LIGHTS = make_list();
	LIGHTMAP = load_resource("textures/lightmap.png");
}

void reset_lights()
{
	list_node *c;
	for (c = LIGHTS->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((lightsource *) c->data) != NULL) {
			free((lightsource *) c->data);
		}
	}
	LIGHTS = make_list();
}

lightsource *make_lightsource(int x, int y, int dim, int intensity, color c)
{
	lightsource *l = (lightsource *) malloc(sizeof(lightsource));
	l->x = x;
	l->y = y;
	l->dim = dim;
	l->intensity = intensity;
	l->c = c;
	return l;
}

void spawn_lightsource(lightsource *l)
{
	insert_list(LIGHTS, (void *) l);
}

void draw_lights()
{
	int w, h;
	w = LEVEL_MAX_DIM * TILE_DIM + 128;
	h = LEVEL_MAX_DIM * TILE_DIM + 128;

	glPushMatrix();
	glTranslatef(-64, -64, 0);
	glColor4f(0, 0, 0, get_current_level()->ambience);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(w, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(w, h, 0);
		glTexCoord2i(0, 1); glVertex3f(0, h, 0);
	glEnd();
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
			glScalef(xdim / 128, ydim / 128, 0);
			glColor3f(l->c.r, l->c.g, l->c.b);
			glBindTexture(GL_TEXTURE_2D, LIGHTMAP->texture);
			for (numtimes = l->intensity; numtimes > 0; numtimes--) {
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				glBindBuffer(GL_ARRAY_BUFFER, LIGHTMAP->vertex_handler);
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
