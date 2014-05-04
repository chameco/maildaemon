#include "projectile.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "weapon.h"
#include "player.h"
#include "entity.h"
#include "level.h"
#include "fx.h"

static list_node *PROJECTILES;

SCM __api_spawn_projectile(SCM c, SCM x, SCM y, SCM xv, SCM yv, SCM w, SCM h,
		SCM longevity, SCM spawned_by, SCM dmg)
{
	color col = *((color *) SCM_SMOB_DATA(c));
	scm_gc_free((color *) c, sizeof(color), "color");
	weapon *sb = (weapon *) SCM_SMOB_DATA(spawned_by);
	spawn_projectile(col, scm_to_int(x), scm_to_int(y), scm_to_int(xv), scm_to_int(yv),
			scm_to_int(w), scm_to_int(h), scm_to_int(longevity), sb, scm_to_int(dmg));
	return SCM_BOOL_F;
}

void initialize_projectile()
{
	PROJECTILES = make_list();

	scm_c_define_gsubr("spawn-projectile", 10, 0, 0, __api_spawn_projectile);
}

void reset_projectile()
{
	list_node *c;
	for (c = PROJECTILES->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((projectile *) c->data) != NULL) {
			free((projectile *) c->data);
		}
	}
	PROJECTILES = make_list();
}

void spawn_projectile(color c, int x, int y, int xv, int yv, int w, int h,
		int longevity, weapon *spawned_by, int dmg)
{
	projectile *p = malloc(sizeof(projectile));
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

	int xmin = (p->x/TILE_DIM)-2;
	int ymin = (p->y/TILE_DIM)-2;
	int xmax = (p->x/TILE_DIM)+2;
	int ymax = (p->y/TILE_DIM)+2;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_tile(x, y)) {
				b.x = x*32;
				b.y = y*32;
				b.w = b.h = TILE_DIM;
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
	glScalef(p->w, p->h, 1);

	glColor4f(p->c.r, p->c.g, p->c.b, p->c.a);
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
