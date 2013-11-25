#include "projectile.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "worldgen.h"
#include "player.h"
#include "entity.h"
#include "level.h"
#include "fx.h"

list_node *PROJECTILES;
vertex PROJECTILE_VERTICES[256][256][4];
GLuint PROJECTILE_VERTEX_HANDLERS[256][256] = {{0}};

void initialize_projectile()
{
	PROJECTILES = make_list();
}

void reset_projectile()
{
	list_node *c;
	for (c = PROJECTILES->next; c->next != NULL; c = c->next) {
		if (((projectile *) c->data) != NULL) {
			free((projectile *) c->data);
		}
	}
	PROJECTILES = make_list();
}

void spawn_projectile(color c, int x, int y, int xv, int yv, int w, int h,
		int longevity, weapon *spawned_by, int dmg)
{
	projectile *p = (projectile *) malloc(sizeof(projectile));
	p->c = c;
	p->x = x;
	p->y = y;
	p->w = w;
	p->h = h;
	p->xv = xv;
	p->yv = yv;
	p->longevity = longevity;
	p->spawned_by = spawned_by;
	p->dmg = dmg;
	if (PROJECTILE_VERTEX_HANDLERS[p->w][p->h] == 0) {
		PROJECTILE_VERTICES[p->w][p->h][0].x = 0;
		PROJECTILE_VERTICES[p->w][p->h][0].y = 0;

		PROJECTILE_VERTICES[p->w][p->h][1].x = p->w;
		PROJECTILE_VERTICES[p->w][p->h][1].y = 0;

		PROJECTILE_VERTICES[p->w][p->h][2].x = p->w;
		PROJECTILE_VERTICES[p->w][p->h][2].y = p->h;

		PROJECTILE_VERTICES[p->w][p->h][3].x = 0;
		PROJECTILE_VERTICES[p->w][p->h][3].y = p->h;

		PROJECTILE_VERTICES[p->w][p->h][0].s = 0;
		PROJECTILE_VERTICES[p->w][p->h][0].t = 0;

		PROJECTILE_VERTICES[p->w][p->h][1].s = 1;
		PROJECTILE_VERTICES[p->w][p->h][1].t = 0;

		PROJECTILE_VERTICES[p->w][p->h][2].s = 1;
		PROJECTILE_VERTICES[p->w][p->h][2].t = 1;

		PROJECTILE_VERTICES[p->w][p->h][3].s = 0;
		PROJECTILE_VERTICES[p->w][p->h][3].t = 1;

		glGenBuffers(1, &PROJECTILE_VERTEX_HANDLERS[p->w][p->h]);
		glBindBuffer(GL_ARRAY_BUFFER, PROJECTILE_VERTEX_HANDLERS[p->w][p->h]);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), PROJECTILE_VERTICES[p->w][p->h], GL_STATIC_DRAW);
	}
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

	if (p->spawned_by != get_player_weapon()) {
		b.x = get_player_x();
		b.y = get_player_y();
		b.w = get_player_w();
		b.h = get_player_h();
		if (!check_collision(a, b)) {
			hit_player(p->dmg);
			destroy_projectile(p);
			return;
		}

	}

	int blockdim = get_block_dim();
	int xmin = (p->x/blockdim)-2;
	int ymin = (p->y/blockdim)-2;
	int xmax = (p->x/blockdim)+2;
	int ymax = (p->y/blockdim)+2;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_block(get_current_region(get_world()), x, y)) {
				b.x = x*32;
				b.y = y*32;
				b.w = b.h = blockdim;
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
		if (((entity *) c->data)->weapon != p->spawned_by &&
				((entity *) c->data) != NULL) {
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

void draw_one_projectile(projectile *p)
{
	glPushMatrix();
	glTranslatef(p->x, p->y, 0);

	glColor4f(p->c.r, p->c.g, p->c.b, p->c.a);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, PROJECTILE_VERTEX_HANDLERS[p->w][p->h]);
	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
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
	glBindTexture(GL_TEXTURE_2D, 0);
	list_node *c;
	for (c = PROJECTILES; c->next != NULL; c = c->next) {
		if (((projectile *) c->data) != NULL) {
			draw_one_projectile((projectile *) c->data);
		}
	}
}
