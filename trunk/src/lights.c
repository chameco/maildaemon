#include "lights.h"

list_node *LIGHTS;

GLuint LIGHTMAP;
GLuint LIGHT_OVERLAY;
vertex LIGHT_VERTICES[4];
GLuint LIGHT_VERTEX_HANDLER;

void initialize_lights()
{
	LIGHTS = make_list();
	LIGHTMAP = load_texture("textures/lightmap.png");

	LIGHT_VERTICES[0].x = 0;
	LIGHT_VERTICES[0].y = 0;

	LIGHT_VERTICES[1].x = 128;
	LIGHT_VERTICES[1].y = 0;

	LIGHT_VERTICES[2].x = 128;
	LIGHT_VERTICES[2].y = 128;

	LIGHT_VERTICES[3].x = 0;
	LIGHT_VERTICES[3].y = 128;

	LIGHT_VERTICES[0].s = 0;
	LIGHT_VERTICES[0].t = 0;

	LIGHT_VERTICES[1].s = 1;
	LIGHT_VERTICES[1].t = 0;

	LIGHT_VERTICES[2].s = 1;
	LIGHT_VERTICES[2].t = 1;

	LIGHT_VERTICES[3].s = 0;
	LIGHT_VERTICES[3].t = 1;

	glGenBuffers(1, &LIGHT_VERTEX_HANDLER);
	glBindBuffer(GL_ARRAY_BUFFER, LIGHT_VERTEX_HANDLER);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), LIGHT_VERTICES, GL_STATIC_DRAW);
}

void reset_lights()
{
	list_node *c;
	for (c = LIGHTS->next; c->next != NULL; c = c->next) {
		if (((lightsource *) c->data) != NULL) {
			free((lightsource *) c->data);
		}
	}
	LIGHTS = make_list();
}

lightsource *create_lightsource(int x, int y, int dim, int intensity, color c)
{
	lightsource *l = (lightsource *) malloc(sizeof(lightsource));
	l->x = x;
	l->y = y;
	l->dim = dim;
	l->intensity = intensity;
	l->c = c;
	return l;
}

void trace_lightsource(lightsource *l)
{
	insert_list(LIGHTS, (void *) l);
}

void spawn_lightsource(int x, int y, int dim, int intensity, color c)
{
	lightsource *l = create_lightsource(x, y, dim, intensity, c);
	trace_lightsource(l);
}

void draw_lights()
{
	int w, h;
	w = get_current_level()->width * get_block_dim() + 128;
	h = get_current_level()->height * get_block_dim() + 128;

	glPushMatrix();
	glTranslatef(-64, -64, 0);
	glColor4f(0, 0, 0, get_current_level_ambience());
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(w, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(w, h, 0);
		glTexCoord2i(0, 1); glVertex3f(0, h, 0);
	glEnd();
	glPopMatrix();

	glBlendFunc(GL_DST_COLOR, GL_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	list_node *c;
	lightsource *l;
	int numtimes;
	int xdim, ydim;
	for (c = LIGHTS->next; c->next != NULL; c = c->next) {
		if (((lightsource *) c->data) != NULL) {
			l = (lightsource *) c->data;
			glPushMatrix();
			glTranslatef(l->x, l->y, 0);
			xdim = l->dim + (random() % 16);
			ydim = l->dim + (random() % 16);
			glScalef(xdim / 128, ydim / 128, 0);
			glColor3f(l->c.r, l->c.g, l->c.b);
			glBindTexture(GL_TEXTURE_2D, LIGHTMAP);
			for (numtimes = l->intensity; numtimes > 0; numtimes--) {
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				glBindBuffer(GL_ARRAY_BUFFER, LIGHT_VERTEX_HANDLER);
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
