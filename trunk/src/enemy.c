#include "enemy.h"

GLuint ENEMY_TEXTURES[256];
list_node *ENEMIES;

list_node *get_enemies()
{
	return ENEMIES;
}

void initialize_enemies()
{
	ENEMIES = make_list();
	ENEMY_TEXTURES[0] = load_texture("textures/enemies/slime.png");
}

void spawn_enemy(int x, int y, int w, int h, int tex, int health, int speed, int collidedmg)
{
	enemy *e = (enemy *) malloc(sizeof(enemy));
	e->x = x;
	e->y = y;
	e->w = w;
	e->h = h;
	e->tex = tex;
	e->health = health;
	e->speed = speed;
	e->collidedmg = collidedmg;
	insert_list(ENEMIES, (void *) e);
}

void hit_enemy(enemy *e, int dmg)
{
	debug("hit_enemy: e->health=%d, dmg=%d", e->health, dmg);
	e->health -= dmg;
	if (e->health <= 0) {
		remove_list(ENEMIES, (void *) e);
	}
}

void collide_enemy(enemy *e)
{
	hit_player(e->collidedmg);
}

void move_enemy_north(enemy *e)
{
	SDL_Rect a = {
		e->x,
		e->y - e->speed,
		e->w,
		e->h
	};
	move_enemy_worker(e, a);
}

void move_enemy_south(enemy *e)
{
	SDL_Rect a = {
		e->x,
		e->y + e->speed,
		e->w,
		e->h
	};
	move_enemy_worker(e, a);
}

void move_enemy_west(enemy *e)
{
	SDL_Rect a = {
		e->x - e->speed,
		e->y,
		e->w,
		e->h
	};
	move_enemy_worker(e, a);
}

void move_enemy_east(enemy *e)
{
	SDL_Rect a = {
		e->x + e->speed,
		e->y,
		e->w,
		e->h
	};
	move_enemy_worker(e, a);
}

void move_enemy_worker(enemy *e, SDL_Rect a)
{
	SDL_Rect b;

	b.x = get_player_x();
	b.y = get_player_y();
	b.w = get_player_w();
	b.h = get_player_h();
	if (!check_collision(a, b)) {
		collide_enemy(e);
		return;
	}

	level *cur = get_current_level();
	int blockdim = get_block_dim();
	int xmin = (e->x/blockdim)-5;
	xmin = (xmin >= 0) ? xmin : 0;
	int ymin = (e->y/blockdim)-5;
	ymin = (ymin >= 0) ? ymin : 0;
	int xmax = (e->x/blockdim)+5;
	xmax = (xmax >= cur->width) ? xmax : cur->width-1;
	int ymax = (e->y/blockdim)+5;
	ymax = (ymax >= cur->height) ? ymax : cur->width-1;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_block(cur->dimensions[x][y])) {
				b.x = x*32;
				b.y = y*32;
				b.w = b.h = blockdim;
				if (!check_collision(a, b)) {
					return;
				}
			}
		}
	}
	e->x = a.x;
	e->y = a.y;
}

void update_enemies()
{
	list_node *c;
	for (c = ENEMIES->next; c->next != NULL; c = c->next) {
		if (((enemy *) c->data) != NULL) {
			if ((random() % 11) > 5) {
				if ((random() % 11) > 5) {
					move_enemy_north((enemy *) c->data);
				} else {
					move_enemy_south((enemy *) c->data);
				}
			} else {
				if ((random() % 11) > 5) {
					move_enemy_west((enemy *) c->data);
				} else {
					move_enemy_east((enemy *) c->data);
				}
			}
		}
	}
}

void draw_enemy(enemy *e)
{
	glPushMatrix();
	glTranslatef(e->x, e->y, 0);
	
	glBindTexture(GL_TEXTURE_2D, ENEMY_TEXTURES[e->tex]);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(e->w, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(e->w, e->h, 0);
		glTexCoord2i(0, 1); glVertex3f(0, e->h, 0);
	glEnd();
	glPopMatrix();
}

void draw_enemies()
{
	list_node *c;
	for (c = ENEMIES; c->next != NULL; c = c->next) {
		if (((enemy *) c->data) != NULL) {
			draw_enemy((enemy *) c->data);
		}
	}
}
