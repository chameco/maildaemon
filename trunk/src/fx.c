#include "fx.h"

list_node *EFFECTS;
int PARTICLE_WIDTH = 2;
int PARTICLE_HEIGHT = 2;
vertex EFFECT_VERTICES[256][4];
GLuint EFFECT_VERTEX_HANDLERS[256] = {0};

void initialize_effects()
{
	EFFECTS = make_list();
}

void spawn_effect(float r, float g, float b, float a,
		int x, int y, int dim,
		int radius, int speed)
{
	effect *e = (effect *) malloc(sizeof(effect));
	e->r = r;
	e->g = g;
	e->b = b;
	e->a = a;
	e->x = x;
	e->y = y;
	e->dim = dim;
	e->radius = radius;
	e->speed = speed;
	e->cur = 0;
	if (EFFECT_VERTEX_HANDLERS[e->dim] == 0) {
		EFFECT_VERTICES[e->dim][0].x = 0;
		EFFECT_VERTICES[e->dim][0].y = 0;

		EFFECT_VERTICES[e->dim][1].x = e->dim;
		EFFECT_VERTICES[e->dim][1].y = 0;

		EFFECT_VERTICES[e->dim][2].x = e->dim;
		EFFECT_VERTICES[e->dim][2].y = e->dim;

		EFFECT_VERTICES[e->dim][3].x = 0;
		EFFECT_VERTICES[e->dim][3].y = e->dim;

		glGenBuffers(1, &EFFECT_VERTEX_HANDLERS[e->dim]);
		glBindBuffer(GL_ARRAY_BUFFER, EFFECT_VERTEX_HANDLERS[e->dim]);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), EFFECT_VERTICES[e->dim], GL_STATIC_DRAW);
	}
	insert_list(EFFECTS, (void *) e);
}

void update_effects()
{
	list_node *c;
	for (c = EFFECTS->next; c->next != NULL; c = c->next) {
		if (((effect *) c->data) != NULL) {
			if (((effect *) c->data)->cur < ((effect *) c->data)->radius) {
				((effect *) c->data)->cur += ((effect *) c->data)->speed;
			} else {
				remove_list(EFFECTS, c->data);
			}
		}
	}
}

void draw_particle(effect *e, int xdiff, int ydiff)
{
	glPushMatrix();

	glTranslatef(e->x+xdiff, e->y+ydiff, 0);

	glColor4f(e->r, e->g, e->b, e->a);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, EFFECT_VERTEX_HANDLERS[e->dim]);
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void draw_effect(effect *e)
{
	draw_particle(e, -(e->cur)/2, -(e->cur)/2);
	draw_particle(e, 0, -(e->cur));
	draw_particle(e, e->cur/2, -(e->cur)/2);
	draw_particle(e, e->cur, 0);
	draw_particle(e, e->cur/2, e->cur/2);
	draw_particle(e, 0, e->cur);
	draw_particle(e, -(e->cur)/2, e->cur/2);
	draw_particle(e, -(e->cur), 0);
}

void draw_effects()
{
	list_node *c;
	for (c = EFFECTS->next; c->next != NULL; c = c->next) {
		if (((effect *) c->data) != NULL) {
			draw_effect((effect *) c->data);
		}
	}
}
