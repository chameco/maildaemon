#include "character.h"

int PLAYER_X = 0;
int PLAYER_Y = 0;
int PLAYER_X_VELOCITY = 0;
int PLAYER_Y_VELOCITY = 0;
int PLAYER_MOVE_SPEED = 8;//Pixels per second
int PLAYER_WIDTH = 32;
int PLAYER_HEIGHT = 32;
direction PLAYER_FACING = 0;
int PLAYER_HEALTH = 100;
GLuint PLAYER_TEXTURES[4];

void initialize_player()
{
	PLAYER_TEXTURES[NORTH] = load_texture("textures/player/n.png");
	PLAYER_TEXTURES[SOUTH] = load_texture("textures/player/s.png");
	PLAYER_TEXTURES[WEST] = load_texture("textures/player/w.png");
	PLAYER_TEXTURES[EAST] = load_texture("textures/player/e.png");
}

void draw_player()
{
	glPushMatrix();
	glTranslatef(PLAYER_X, PLAYER_Y, 0);
	glBindTexture(GL_TEXTURE_2D, PLAYER_TEXTURES[PLAYER_FACING]);

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(PLAYER_WIDTH, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(PLAYER_WIDTH, PLAYER_HEIGHT, 0);
		glTexCoord2i(0, 1); glVertex3f(0, PLAYER_HEIGHT, 0);
	glEnd();
	glPopMatrix();
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

void hit_player(int dmg)
{
	PLAYER_HEALTH -= dmg;
}

void warp_player(int x, int y)
{
	PLAYER_X = x;
	PLAYER_Y = y;
}

void shoot_player_weapon(int pressed)
{
	if (pressed) {
		spawn_projectile(1.0, 0.0, 0.0, 1.0,
				PLAYER_FACING, PLAYER_X, PLAYER_Y+10, 8, 8,
				1, 2, 16);
		spawn_projectile(0.0, 0.0, 1.0, 1.0,
				PLAYER_FACING, PLAYER_X+24, PLAYER_Y, 8, 8,
				1, 2, 16);
	}
}

void move_player_north(int pressed)
{
	if (pressed) {
		PLAYER_Y_VELOCITY = -PLAYER_MOVE_SPEED;
		PLAYER_FACING = NORTH;
	} else {
		PLAYER_Y_VELOCITY = 0;
	}
}

void move_player_south(int pressed)
{
	if (pressed) {
		PLAYER_Y_VELOCITY = PLAYER_MOVE_SPEED;
		PLAYER_FACING = SOUTH;
	} else {
		PLAYER_Y_VELOCITY = 0;
	}
}

void move_player_west(int pressed)
{
	if (pressed) {
		PLAYER_X_VELOCITY = -PLAYER_MOVE_SPEED;
		PLAYER_FACING = WEST;
	} else {
		PLAYER_X_VELOCITY = 0;
	}
}

void move_player_east(int pressed)
{
	if (pressed) {
		PLAYER_X_VELOCITY = PLAYER_MOVE_SPEED;
		PLAYER_FACING = EAST;
	} else {
		PLAYER_X_VELOCITY = 0;
	}
}

void update_player()
{
	int tempx = PLAYER_X + PLAYER_X_VELOCITY;
	int tempy = PLAYER_Y + PLAYER_Y_VELOCITY;
	SDL_Rect player = {
		PLAYER_X,
		PLAYER_Y,
		PLAYER_WIDTH,
		PLAYER_HEIGHT
	};
	SDL_Rect b;
	int shouldmovex = 1;
	int shouldmovey = 1;

	level *cur = get_current_level();
	int blockdim = get_block_dim();
	int xmin = (PLAYER_X/blockdim)-5;
	xmin = (xmin >= 0) ? xmin : 0;
	int ymin = (PLAYER_Y/blockdim)-5;
	ymin = (ymin >= 0) ? ymin : 0;
	int xmax = (PLAYER_X/blockdim)+5;
	xmax = (xmax >= cur->width) ? xmax : cur->width-1;
	int ymax = (PLAYER_Y/blockdim)+5;
	ymax = (ymax >= cur->height) ? ymax : cur->width-1;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_block(cur->dimensions[x][y])) {
				b.x = x*32;
				b.y = y*32;
				b.w = b.h = blockdim;
				player.x = PLAYER_X+PLAYER_X_VELOCITY;
				shouldmovex = check_collision(player, b);
				player.x = PLAYER_X;
				player.y = PLAYER_Y+PLAYER_Y_VELOCITY;
				shouldmovey = check_collision(player, b);
				player.y = PLAYER_Y;
				if (!shouldmovex || !shouldmovey) {
					goto end;
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
			player.x = PLAYER_X+PLAYER_X_VELOCITY;
			shouldmovex = check_collision(player, b);
			player.x = PLAYER_X;
			player.y = PLAYER_Y+PLAYER_Y_VELOCITY;
			shouldmovey = check_collision(player, b);
			player.y = PLAYER_Y;
			if (!shouldmovex || !shouldmovey) {
				player.x = PLAYER_X;
				player.y = PLAYER_Y;
				if (!check_collision(player, b)) {
					collide_enemy((enemy *) c->data);
				}
				goto end;
			}
		}
	}

end:
	if (shouldmovex) {
		PLAYER_X = tempx;
	} else {
		PLAYER_X_VELOCITY = 0;
	}
	if (shouldmovey) {
		PLAYER_Y = tempy;
	} else {
		PLAYER_Y_VELOCITY = 0;
	}
}