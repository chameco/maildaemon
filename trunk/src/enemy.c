#include "enemy.h"

GLuint ENEMY_TEXTURES[256];
list_node *ENEMIES;
vertex ENEMY_VERTICES[256][4];
GLuint ENEMY_VERTEX_HANDLERS[256];

list_node *get_enemies()
{
	return ENEMIES;
}

void set_enemies(list_node *enemies)
{
	ENEMIES = enemies;
}

void initialize_enemy(int i, char *path, int w, int h)
{
	ENEMY_TEXTURES[i] = load_texture(path);

	ENEMY_VERTICES[i][0].x = 0;
	ENEMY_VERTICES[i][0].y = 0;

	ENEMY_VERTICES[i][1].x = w;
	ENEMY_VERTICES[i][1].y = 0;

	ENEMY_VERTICES[i][2].x = w;
	ENEMY_VERTICES[i][2].y = h;

	ENEMY_VERTICES[i][3].x = 0;
	ENEMY_VERTICES[i][3].y = h;

	ENEMY_VERTICES[i][0].s = 0;
	ENEMY_VERTICES[i][0].t = 0;

	ENEMY_VERTICES[i][1].s = 1;
	ENEMY_VERTICES[i][1].t = 0;

	ENEMY_VERTICES[i][2].s = 1;
	ENEMY_VERTICES[i][2].t = 1;

	ENEMY_VERTICES[i][3].s = 0;
	ENEMY_VERTICES[i][3].t = 1;

	glGenBuffers(1, &ENEMY_VERTEX_HANDLERS[i]);
	glBindBuffer(GL_ARRAY_BUFFER, ENEMY_VERTEX_HANDLERS[i]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), ENEMY_VERTICES[i], GL_STATIC_DRAW);
}

void initialize_enemies()
{
	ENEMIES = make_list();
	initialize_enemy(0, "textures/enemies/slime.png", 32, 32);
	initialize_enemy(1, "textures/enemies/wizard.png", 32, 32);
}

void reset_enemies()
{
	list_node *c;
	for (c = ENEMIES->next; c->next != NULL; c = c->next) {
		if (((enemy *) c->data) != NULL) {
			free((enemy *) c->data);
		}
	}
	ENEMIES = make_list();
}

void spawn_enemy(int x, int y, int w, int h,
		int tex,
		int health, int speed, int attk, double expval, int pspeed, 
		float pr, float pg, float pb, float pa, int pdim, int edim)
{
	enemy *e = (enemy *) malloc(sizeof(enemy));
	e->x = x;
	e->y = y;
	e->w = w;
	e->h = h;
	e->tex = tex;
	e->health = health;
	e->speed = speed;
	e->attk = attk;
	e->expval = expval;
	e->pspeed = pspeed;
	e->pr = pr;
	e->pg = pg;
	e->pb = pb;
	e->pa = pa;
	e->pdim = pdim;
	e->edim = edim;
	insert_list(ENEMIES, (void *) e);
}

void hit_enemy(enemy *e, int dmg)
{
	e->health -= dmg;
	if (e->health <= 0) {
		remove_list(ENEMIES, (void *) e);
		give_player_exp(e->expval);
	}
}

void collide_enemy(enemy *e)
{
	hit_player(e->attk);
}

void shoot_enemy_weapon(enemy *e, direction d)
{
	spawn_projectile(e->pr, e->pg, e->pb, e->pa,
			d, e->x, e->y, e->pdim, (void *)e,
			e->attk, e->pspeed, e->edim);
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
	int xmin = (e->x/blockdim)-2;
	xmin = (xmin >= 0) ? xmin : 0;
	int ymin = (e->y/blockdim)-2;
	ymin = (ymin >= 0) ? ymin : 0;
	int xmax = (e->x/blockdim)+2;
	xmax = (xmax >= cur->width) ? xmax : cur->width-1;
	int ymax = (e->y/blockdim)+2;
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
			if (!(random() % 11)) {
				shoot_enemy_weapon((enemy *) c->data, random() % 4);
			}
		}
	}
}

void draw_enemy(enemy *e)
{
	glPushMatrix();
	glTranslatef(e->x, e->y, 0);
	
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, ENEMY_TEXTURES[e->tex]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, ENEMY_VERTEX_HANDLERS[e->tex]);
		glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
		glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

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
