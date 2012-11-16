#include "projectile.h"

list_node *PROJECTILES;

void initialize_projectiles()
{
	PROJECTILES = make_list();
}

void spawn_projectile(double r, double g, double b, double a,
		direction d, int x, int y, int w, int h,
		int sbp, int dmg, int speed)
{
	projectile *p = (projectile *) malloc(sizeof(projectile));
	p->r = r;
	p->g = g;
	p->b = b;
	p->a = a;
	p->x = x;
	p->y = y;
	p->w = w;
	p->h = h;
	p->xv = 0;
	p->yv = 0;
	p->sbp = sbp;
	p->dmg = dmg;
	switch (d) {
		case NORTH:
			p->yv = -speed;
			break;
		case SOUTH:
			p->yv = speed;
			break;
		case WEST:
			p->xv = -speed;
			break;
		case EAST:
			p->xv = speed;
			break;
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

	if (!p->sbp) {
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
	int xmin = (p->x/blockdim)-5;
	xmin = (xmin >= 0) ? xmin : 0;
	int ymin = (p->y/blockdim)-5;
	ymin = (ymin >= 0) ? ymin : 0;
	int xmax = (p->x/blockdim)+5;
	xmax = (xmax >= cur->width) ? xmax : cur->width-1;
	int ymax = (p->y/blockdim)+5;
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
		if (((enemy *) c->data) != NULL) {
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
}

void destroy_projectile(projectile *p)
{
	remove_list(PROJECTILES, p);
}


void draw_projectile(projectile *p)
{
	glPushMatrix();
	glTranslatef(p->x, p->y, 0);
	
	glColor4f(p->r, p->g, p->b, p->a);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(p->w, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(p->w, p->h, 0);
		glTexCoord2i(0, 1); glVertex3f(0, p->h, 0);
	glEnd();
	glPopMatrix();
}


void update_projectiles()
{
	list_node *c;
	for (c = PROJECTILES->next; c->next != NULL; c = c->next) {
		if (((projectile *) c->data) != NULL) {
			((projectile *) c->data)->x += ((projectile *) c->data)->xv;
			((projectile *) c->data)->y += ((projectile *) c->data)->yv;
			check_projectile_collisions((projectile *) c->data);
		}
	}
}

void draw_projectiles()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	list_node *c;
	for (c = PROJECTILES; c->next != NULL; c = c->next) {
		if (((projectile *) c->data) != NULL) {
			draw_projectile((projectile *) c->data);
		}
	}
}
