#include "entity.h"

GLuint ENTITY_TEXTURES[256];
list_node *ENTITIES;
vertex ENTITY_VERTICES[256][4];
GLuint ENTITY_VERTEX_HANDLERS[256];

list_node *get_entities()
{
	return ENTITIES;
}

void initialize_one_entity(int i, char *path, int w, int h)
{
	ENTITY_TEXTURES[i] = load_texture(path);

	ENTITY_VERTICES[i][0].x = 0;
	ENTITY_VERTICES[i][0].y = 0;

	ENTITY_VERTICES[i][1].x = w;
	ENTITY_VERTICES[i][1].y = 0;

	ENTITY_VERTICES[i][2].x = w;
	ENTITY_VERTICES[i][2].y = h;

	ENTITY_VERTICES[i][3].x = 0;
	ENTITY_VERTICES[i][3].y = h;

	ENTITY_VERTICES[i][0].s = 0;
	ENTITY_VERTICES[i][0].t = 0;

	ENTITY_VERTICES[i][1].s = 1;
	ENTITY_VERTICES[i][1].t = 0;

	ENTITY_VERTICES[i][2].s = 1;
	ENTITY_VERTICES[i][2].t = 1;

	ENTITY_VERTICES[i][3].s = 0;
	ENTITY_VERTICES[i][3].t = 1;

	glGenBuffers(1, &ENTITY_VERTEX_HANDLERS[i]);
	glBindBuffer(GL_ARRAY_BUFFER, ENTITY_VERTEX_HANDLERS[i]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), ENTITY_VERTICES[i], GL_STATIC_DRAW);
}

void initialize_entity()
{
	ENTITIES = make_list();
	initialize_one_entity(0, "textures/up.png", 32, 32);
	initialize_one_entity(1, "textures/down.png", 32, 32);
}

void reset_entities()
{
	list_node *c;
	for (c = ENTITIES->next; c->next != NULL; c = c->next) {
		if (((enemy *) c->data) != NULL) {
			free((enemy *) c->data);
		}
	}
	ENTITIES = make_list();
}

void spawn_entity(int id, int x, int y, int w, int h)
{
	entity *e = (entity *) malloc(sizeof(entity));
	e->id = id;
	e->x = x;
	e->y = y;
	e->w = w;
	e->h = h;
	insert_list(ENTITIES, (void *) e);
}

int collide_entity(entity *e)
{
	switch (e->id) {
		case 0: //UP STAIRS
			load_level(get_current_level_index()+1);
			return 1;
			break;
		case 1: //DOWN STAIRS
			load_level(get_current_level_index()-1);
			return 1;
			break;
	}
	return 0;
}

void hit_entity(entity *e, int dmg)
{
}

void update_entity()
{
}

void draw_one_entity(entity *e)
{
	glPushMatrix();
	glTranslatef(e->x, e->y, 0);
	
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, ENTITY_TEXTURES[e->id]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, ENTITY_VERTEX_HANDLERS[e->id]);
		glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
		glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void draw_entity()
{
	list_node *c;
	for (c = ENTITIES; c->next != NULL; c = c->next) {
		if (((entity *) c->data) != NULL) {
			draw_one_entity((entity *) c->data);
		}
	}
}
