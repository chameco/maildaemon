#include "fx.h"

list_node *EFFECTS;
int PARTICLE_WIDTH = 2;
int PARTICLE_HEIGHT = 2;
vertex EFFECT_VERTICES[256][4];
GLuint EFFECT_VERTEX_HANDLERS[256] = {0};

void initialize_fx()
{
	EFFECTS = make_list();
}

void reset_fx()
{
	list_node *c;
	for (c = EFFECTS->next; c->next != NULL; c = c->next) {
		if (((effect *) c->data) != NULL) {
			free((effect *) c->data);
		}
	}
	EFFECTS = make_list();
}

void spawn_fx(etype type, color col,
		int x, int y, int dim,
		int radius, int speed)
{
	effect *e = (effect *) malloc(sizeof(effect));
	e->type = type;
	e->c = col;
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
	int c;
	switch (e->type) {
		case EXPLOSION:
			break;
		case SMOKE_CONST:
		case SMOKE:
			e->statelen = 25;
			for (c = 0; c < e->statelen; c++) {
				if (random() % 2) {
					e->state[c][0] = random() % e->radius/4;
				} else {
					e->state[c][0] = -(random() % e->radius/4);
				}
				e->state[c][1] = -(random() % e->radius);
				e->state[c][2] = 1;
			}
			break;
	}
	insert_list(EFFECTS, (void *) e);
}

void update_fx()
{
	list_node *c;
	effect *e;
	int counter;
	for (c = EFFECTS->next; c->next != NULL; c = c->next) {
		e = (effect *) c->data;
		if (e != NULL) {
			switch (e->type) {
				case EXPLOSION:
					if (e->cur < e->radius) {
						e->cur += e->speed;
					} else {
						remove_list(EFFECTS, c->data);
					}
					break;
				case SMOKE_CONST:
					for (counter = 0; counter < e->statelen; counter++) {
						if (random() % 2) {
							e->state[counter][0] += random() % 3;
						} else {
							e->state[counter][0] -= random() % 3;
						}
						if (e->state[counter][1] >= -(e->radius)) {
							e->state[counter][1] -= random() % 3;
						} else {
							e->state[counter][1] = 0;
							if (random() % 2) {
								e->state[counter][0] = random() % e->radius/4;
							} else {
								e->state[counter][0] = -(random() % e->radius/4);
							}
						}
					}
					break;
				case SMOKE:
					e->cur += e->speed;
					if (e->cur >= 100) {
						remove_list(EFFECTS, c->data);
					}
					for (counter = 0; counter < e->statelen; counter++) {
						if (e->state[counter][2]) {
							if (random() % 2) {
								e->state[counter][0] += random() % 3;
							} else {
								e->state[counter][0] -= random() % 3;
							}
							if (e->state[counter][1] >= -(e->radius)) {
								e->state[counter][1] -= random() % 5;
							} else {
								e->state[counter][2] = 0;
							}
						}
					}
					break;
			}
		}
	}
}

void draw_particle(effect *e, int xdiff, int ydiff)
{
	glPushMatrix();

	glTranslatef(e->x+xdiff, e->y+ydiff, 0);

	glColor4f(e->c.r, e->c.g, e->c.b, e->c.a);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, EFFECT_VERTEX_HANDLERS[e->dim]);
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
	factor = (GLfloat) drand48();
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

	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4f(r, g, b, e->c.a);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, EFFECT_VERTEX_HANDLERS[e->dim]);
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
			draw_particle(e, -(e->cur)/2, -(e->cur)/2);
			draw_particle(e, 0, -(e->cur));
			draw_particle(e, e->cur/2, -(e->cur)/2);
			draw_particle(e, e->cur, 0);
			draw_particle(e, e->cur/2, e->cur/2);
			draw_particle(e, 0, e->cur);
			draw_particle(e, -(e->cur)/2, e->cur/2);
			draw_particle(e, -(e->cur), 0);
			break;
		case SMOKE_CONST:
		case SMOKE:
			for (c = 0; c < e->statelen; c++) {
				if (e->state[c][2]) {
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
