#include "level.h"

level *CURRENT_LEVEL;
GLuint BLOCK_TEXTURES[256];
const int LEVEL_COUNT = 0;
const int BLOCK_DIM = 32;

level *get_current_level()
{
	return CURRENT_LEVEL;
}

int get_block_dim()
{
	return BLOCK_DIM;
}

int is_solid_block(block b)
{
	return b;
}

void initialize_levels()
{
	BLOCK_TEXTURES[FLOOR] = load_texture("textures/floor.png");
	BLOCK_TEXTURES[WALL] = load_texture("textures/wall.png");
}

void load_level(int index)
{
	int x, y, w, h;
	FILE *curlevel;
	char *buffer;
	size_t len = 0;
	char cur;
	char sprintf_fodder[100];
	reset_enemies();
	reset_projectiles();
	CURRENT_LEVEL = (level *) malloc(sizeof(level));
	x = y = w = h = 0;
	sprintf(sprintf_fodder, "levels/level%d", index);
	curlevel = fopen(sprintf_fodder, "r");
	if (curlevel == NULL) {
		sprintf(sprintf_fodder, "%slevels/level%d", DATADIR, index);
		fopen(sprintf_fodder, "r");
	}
	fscanf(curlevel, "%d %d\n", &w, &h);
	CURRENT_LEVEL->width = w;
	CURRENT_LEVEL->height = h;
	for (y = 0; y < h; y++) {
		getline(&buffer, &len, curlevel);
		debug("%s", buffer);
		for (x = 0; x < w; x++) {
			cur = buffer[x];
			switch (cur) {
				case '\n':
					break;
				case 'C':
					warp_player(x*BLOCK_DIM, y*BLOCK_DIM);
				case ' ':
					CURRENT_LEVEL->dimensions[x][y] = FLOOR;
					break;
				case 'S':
					spawn_enemy(x*BLOCK_DIM, y*BLOCK_DIM,
							32, 32, 0, 10, 8, 2, 10);
					CURRENT_LEVEL->dimensions[x][y] = FLOOR;
					break;
				case 'W':
					spawn_enemy(x*BLOCK_DIM, y*BLOCK_DIM,
							32, 32, 1, 100, 8, 10, 100);
					CURRENT_LEVEL->dimensions[x][y] = FLOOR;
					break;
				case '#':
					CURRENT_LEVEL->dimensions[x][y] = WALL;
					break;
				default:
					log_err("Invalid syntax in level%d: %c", index, cur);
			}
		}
	}
}

void draw_block(block b, int x, int y)
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glBindTexture(GL_TEXTURE_2D, BLOCK_TEXTURES[b]);

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
	glTexCoord2i(1, 0); glVertex3f(BLOCK_DIM, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(BLOCK_DIM, BLOCK_DIM, 0);
		glTexCoord2i(0, 1); glVertex3f(0, BLOCK_DIM, 0);
	glEnd();
	glPopMatrix();
}

void draw_current_level()
{
	int x, y;
	glLoadIdentity();
	for (x = 0; x < CURRENT_LEVEL->width; x++) {
		for (y = 0; y < CURRENT_LEVEL->height; y++) {
			draw_block(CURRENT_LEVEL->dimensions[x][y],
					x*BLOCK_DIM,
					y*BLOCK_DIM);
		}
	}
}
