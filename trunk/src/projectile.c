#include "projectile.h"

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
		int longevity, void *sb, int dmg, int edim, GLuint texture)
{
	projectile *p = (projectile *) malloc(sizeof(projectile));
	p->c = c;
	p->x = x;
	p->y = y;
	p->xv = xv;
	p->yv = yv;
	p->w = w;
	p->h = h;
	p->longevity = longevity;
	p->sb = sb;
	p->dmg = dmg;
	p->edim = edim;
	p->texture = texture;
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

	if (p->sb != NULL) {
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

	level *cur = get_current_level();
	int blockdim = get_block_dim();
	int xmin = (p->x/blockdim)-2;
	xmin = (xmin >= 0) ? xmin : 0;
	int ymin = (p->y/blockdim)-2;
	ymin = (ymin >= 0) ? ymin : 0;
	int xmax = (p->x/blockdim)+2;
	xmax = (xmax >= cur->width) ? xmax : cur->width-1;
	int ymax = (p->y/blockdim)+2;
	ymax = (ymax >= cur->height) ? ymax : cur->width-1;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_block(cur->dimensions[x][y])) {
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

	list_node *enemies = get_enemies();
	list_node *c;
	for (c = enemies->next; c->next != NULL; c = c->next) {
		if ((enemy *) c->data != p->sb &&
				((enemy *) c->data) != NULL) {
			b.x = ((enemy *) c->data)->x;
			b.y = ((enemy *) c->data)->y;
			b.w = ((enemy *) c->data)->w;
			b.h = ((enemy *) c->data)->h;
			if (!check_collision(a, b)) {
				hit_enemy((enemy *) c->data, p->dmg);
				destroy_projectile(p);
				return;
			}
		}
	}

	list_node *entities = get_entities();
	for (c = entities->next; c->next != NULL; c = c->next) {
		if ((entity *) c->data != p->sb &&
				((entity *) c->data) != NULL) {
			b.x = ((entity *) c->data)->x;
			b.y = ((entity *) c->data)->y;
			b.w = ((entity *) c->data)->w;
			b.h = ((entity *) c->data)->h;
			if (!check_collision(a, b)) {
				hit_entity((entity *) c->data, p->dmg);
				destroy_projectile(p);
				return;
			}
		}
	}
}

void destroy_projectile(projectile *p)
{
	remove_list(PROJECTILES, p);
	spawn_fx(EXPLOSION, p->c,
			p->x, p->y, p->edim, 50, 10);
}

void draw_one_projectile(projectile *p)
{
	glPushMatrix();
	glTranslatef(p->x, p->y, 0);

	glColor4f(p->c.r, p->c.g, p->c.b, p->c.a);
	glBindTexture(GL_TEXTURE_2D, p->texture);

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
