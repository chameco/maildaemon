#include "player.h"

int PLAYER_X = 0;
int PLAYER_Y = 0;
int NORTH_PRESSED = 0;
int SOUTH_PRESSED = 0;
int WEST_PRESSED = 0;
int EAST_PRESSED = 0;
const int PLAYER_MOVE_SPEED = 8;//Pixels per second
const int DIAG_SPEED = 6;
const int PLAYER_WIDTH = 30;
const int PLAYER_HEIGHT = 30;
int PLAYER_ATTACK = 16;
direction PLAYER_FACING = 0;
double PLAYER_EXP = 0;
double PLAYER_EXP_TO_NEXT = 100;
int PLAYER_LEVEL = 0;
double PLAYER_MAX_HEALTH = 100;
double PLAYER_HEALTH;
double PLAYER_MAX_MAGIC = 100;
double PLAYER_MAGIC;
double PLAYER_REGEN = 0.1;
double PLAYER_MREGEN = 0.1;
int PLAYER_NUMKEYS = 0;
GLuint PLAYER_TEXTURES[4];
GLuint PLAYER_SWORD_TEXTURES[4];
color PLAYER_COLOR;
vertex PLAYER_VERTICES[4];
GLuint PLAYER_VERTEX_HANDLER = 0;

void initialize_player()
{
	PLAYER_HEALTH = PLAYER_MAX_HEALTH;
	PLAYER_MAGIC = PLAYER_MAX_MAGIC;
	PLAYER_COLOR = COLOR_RED;

	PLAYER_TEXTURES[NORTH] = load_texture("textures/player/n.png");
	PLAYER_TEXTURES[SOUTH] = load_texture("textures/player/s.png");
	PLAYER_TEXTURES[WEST] = load_texture("textures/player/w.png");
	PLAYER_TEXTURES[EAST] = load_texture("textures/player/e.png");

	PLAYER_SWORD_TEXTURES[NORTH] = load_texture("textures/swordn.png");
	PLAYER_SWORD_TEXTURES[SOUTH] = load_texture("textures/swords.png");
	PLAYER_SWORD_TEXTURES[WEST] = load_texture("textures/swordw.png");
	PLAYER_SWORD_TEXTURES[EAST] = load_texture("textures/sworde.png");

	debug("n %d s %d e %d w %d", PLAYER_SWORD_TEXTURES[NORTH],
			PLAYER_SWORD_TEXTURES[SOUTH],
			PLAYER_SWORD_TEXTURES[EAST],
			PLAYER_SWORD_TEXTURES[WEST]);

	PLAYER_VERTICES[0].x = 0;
	PLAYER_VERTICES[0].y = 0;

	PLAYER_VERTICES[1].x = PLAYER_WIDTH;
	PLAYER_VERTICES[1].y = 0;

	PLAYER_VERTICES[2].x = PLAYER_WIDTH;
	PLAYER_VERTICES[2].y = PLAYER_HEIGHT;

	PLAYER_VERTICES[3].x = 0;
	PLAYER_VERTICES[3].y = PLAYER_HEIGHT;

	PLAYER_VERTICES[0].s = 0;
	PLAYER_VERTICES[0].t = 0;

	PLAYER_VERTICES[1].s = 1;
	PLAYER_VERTICES[1].t = 0;

	PLAYER_VERTICES[2].s = 1;
	PLAYER_VERTICES[2].t = 1;

	PLAYER_VERTICES[3].s = 0;
	PLAYER_VERTICES[3].t = 1;

	glGenBuffers(1, &PLAYER_VERTEX_HANDLER);
	glBindBuffer(GL_ARRAY_BUFFER, PLAYER_VERTEX_HANDLER);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), PLAYER_VERTICES, GL_STATIC_DRAW);
}

void reset_player()
{
	PLAYER_FACING = 0;
	PLAYER_EXP = 0;
	PLAYER_EXP_TO_NEXT = 100;
	PLAYER_LEVEL = 0;
	PLAYER_HEALTH = PLAYER_MAX_HEALTH;
	PLAYER_MAGIC = PLAYER_MAX_MAGIC;
}

int get_player_x()
{
	return PLAYER_X;
}

int get_player_y()
{
	return PLAYER_Y;
}

int get_player_w()
{
	return PLAYER_WIDTH;
}

int get_player_h()
{
	return PLAYER_HEIGHT;
}

int get_player_facing()
{
	return PLAYER_FACING;
}

double get_player_health()
{
	return PLAYER_HEALTH;
}

double get_player_max_health()
{
	return PLAYER_MAX_HEALTH;
}

double get_player_magic()
{
	return PLAYER_MAGIC;
}

double get_player_max_magic()
{
	return PLAYER_MAX_MAGIC;
}
double get_player_exp()
{
	return PLAYER_EXP;
}

double get_player_exp_to_next()
{
	return PLAYER_EXP_TO_NEXT;
}

int get_player_level()
{
	return PLAYER_LEVEL;
}

void hit_player(int dmg)
{
	PLAYER_HEALTH -= dmg;
}

void give_player_exp(double exp)
{
	PLAYER_EXP += exp;
	if (PLAYER_EXP >= PLAYER_EXP_TO_NEXT) {
		PLAYER_EXP = PLAYER_EXP - PLAYER_EXP_TO_NEXT;
		PLAYER_EXP_TO_NEXT += 50;
		PLAYER_LEVEL += 1;
		PLAYER_HEALTH = PLAYER_MAX_HEALTH;
		PLAYER_MAGIC = PLAYER_MAX_MAGIC;
	}
}

void give_player_key()
{
	PLAYER_NUMKEYS += 1;
}

void warp_player(int x, int y)
{
	PLAYER_X = x;
	PLAYER_Y = y;
}

void shoot_player_weapon(int pressed, direction d)
{
	if (pressed) {
		if (PLAYER_MAGIC >= 4) {
			int xv = 0;
			int yv = 0;
			switch (d) {
				case NORTH:
					yv = -16;
					break;
				case SOUTH:
					yv = 16;
					break;
				case WEST:
					xv = -16;
					break;
				case EAST:
					xv = 16;
					break;
			}
			spawn_projectile(PLAYER_COLOR,
					PLAYER_X+(PLAYER_WIDTH/2)-4, PLAYER_Y+(PLAYER_HEIGHT/2)-4,
					xv, yv, 8, 8, 100, NULL, PLAYER_ATTACK, 4, 0);
			PLAYER_MAGIC -= 4;
		}
	}
}

void move_player_north(int pressed)
{
	if (pressed) {
		NORTH_PRESSED = 1;
		PLAYER_FACING = NORTH;
	} else {
		NORTH_PRESSED = 0;
	}
}

void move_player_south(int pressed)
{
	if (pressed) {
		SOUTH_PRESSED = 1;
		PLAYER_FACING = SOUTH;
	} else {
		SOUTH_PRESSED = 0;
	}
}

void move_player_west(int pressed)
{
	if (pressed) {
		WEST_PRESSED = 1;
		PLAYER_FACING = WEST;
	} else {
		WEST_PRESSED = 0;
	}
}

void move_player_east(int pressed)
{
	if (pressed) {
		EAST_PRESSED = 1;
		PLAYER_FACING = EAST;
	} else {
		EAST_PRESSED = 0;
	}
}

mode update_player()
{
	if (PLAYER_HEALTH <= 0.0) {
		return GAME_OVER_MODE;
	}

	if (PLAYER_HEALTH <= PLAYER_MAX_HEALTH-PLAYER_REGEN) {
		PLAYER_HEALTH += PLAYER_REGEN;
	} else if (PLAYER_HEALTH < PLAYER_MAX_HEALTH) {
		PLAYER_HEALTH = PLAYER_MAX_HEALTH;
	}

	if (PLAYER_MAGIC <= PLAYER_MAX_MAGIC-PLAYER_MREGEN) {
		PLAYER_MAGIC += PLAYER_MREGEN;
	} else if (PLAYER_MAGIC < PLAYER_MAX_MAGIC) {
		PLAYER_MAGIC = PLAYER_MAX_MAGIC;
	}

	int tempx = PLAYER_X;
	int tempy = PLAYER_Y;

	if (NORTH_PRESSED) {
		if (WEST_PRESSED || EAST_PRESSED) {
			tempy -= DIAG_SPEED;
		} else {
			tempy -= PLAYER_MOVE_SPEED;
		}
	}

	if (SOUTH_PRESSED) {
		if (WEST_PRESSED || EAST_PRESSED) {
			tempy += DIAG_SPEED;
		} else {
			tempy += PLAYER_MOVE_SPEED;
		}
	}

	if (WEST_PRESSED) {
		if (NORTH_PRESSED || SOUTH_PRESSED) {
			tempx -= DIAG_SPEED;
		} else {
			tempx -= PLAYER_MOVE_SPEED;
		}
	}

	if (EAST_PRESSED) {
		if (NORTH_PRESSED || SOUTH_PRESSED) {
			tempx += DIAG_SPEED;
		} else {
			tempx += PLAYER_MOVE_SPEED;
		}
	}

	SDL_Rect player = {
		PLAYER_X,
		PLAYER_Y,
		PLAYER_WIDTH,
		PLAYER_HEIGHT
	};
	SDL_Rect b;
	int shouldmovex = 1;
	int shouldmovey = 1;
	int curmovex = 0;
	int curmovey = 0;

	level *cur = get_current_level();
	int blockdim = get_block_dim();
	int xmin = (PLAYER_X/blockdim)-2;
	xmin = (xmin >= 0) ? xmin : 0;
	int ymin = (PLAYER_Y/blockdim)-2;
	ymin = (ymin >= 0) ? ymin : 0;
	int xmax = (PLAYER_X/blockdim)+2;
	xmax = (xmax >= cur->width) ? xmax : cur->width-1;
	int ymax = (PLAYER_Y/blockdim)+2;
	ymax = (ymax >= cur->height) ? ymax : cur->width-1;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_block(cur->dimensions[x][y])) {
				b.x = x*32;
				b.y = y*32;
				b.w = b.h = blockdim;
				player.x = tempx;
				player.y = PLAYER_Y;
				curmovex = check_collision(player, b);
				shouldmovex = shouldmovex ? curmovex : 0;
				player.x = PLAYER_X;
				player.y = tempy;
				curmovey = check_collision(player, b);
				shouldmovey = shouldmovey ? curmovey : 0;
				player.y = PLAYER_Y;
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
			player.x = tempx;
			curmovex = check_collision(player, b);
			shouldmovex = shouldmovex ? curmovex : 0;
			player.x = PLAYER_X;
			player.y = tempy;
			curmovey = check_collision(player, b);
			shouldmovey = shouldmovey ? curmovey : 0;
			player.y = PLAYER_Y;
			if (!curmovex || !curmovey) {
				collide_enemy((enemy *) c->data);
			}
		}
	}

	list_node *entities = get_entities();
	for (c = entities->next; c->next != NULL; c = c->next) {
		if (((entity *) c->data) != NULL) {
			b.x = ((entity *) c->data)->x;
			b.y = ((entity *) c->data)->y;
			b.w = ((entity *) c->data)->w;
			b.h = ((entity *) c->data)->h;
			player.x = tempx;
			curmovex = check_collision(player, b);
			shouldmovex = shouldmovex ? curmovex : 0;
			player.x = PLAYER_X;
			player.y = tempy;
			curmovey = check_collision(player, b);
			shouldmovey = shouldmovey ? curmovey : 0;
			player.y = PLAYER_Y;
			if (!curmovex || !curmovey) {
				player.x = PLAYER_X;
				player.y = PLAYER_Y;
				if (collide_entity((entity *) c->data)) {
					return DRAW_MODE;
				}
			}
		}
	}

	if (shouldmovex) {
		PLAYER_X = tempx;
	}
	if (shouldmovey) {
		PLAYER_Y = tempy;
	}
	return DRAW_MODE;
}

void draw_player()
{
	glPushMatrix();
	glTranslatef(PLAYER_X, PLAYER_Y, 0);
	
	glBindTexture(GL_TEXTURE_2D, PLAYER_TEXTURES[PLAYER_FACING]);

	glColor3f(1.0, 1.0, 1.0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, PLAYER_VERTEX_HANDLER);
		glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
		glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}
