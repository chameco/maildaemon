#include "level.h"

level *CURRENT_LEVEL;
level *LEVELS[256];
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
	int x, y, w, h;
	FILE *curlevel;
	char *buffer;
	size_t len = 0;
	int c;
	char cur;
	char sprintf_fodder[100];
	for (c = 0; c <= LEVEL_COUNT; c++) {
		LEVELS[c] = (level *) malloc(sizeof(level));
		x = y = w = h = 0;
		sprintf(sprintf_fodder, "levels/level%d", c);
		curlevel = fopen(sprintf_fodder, "r");
		fscanf(curlevel, "%d %d\n", &w, &h);
		LEVELS[c]->width = w;
		LEVELS[c]->height = h;
		for (y = 0; y < h; y++) {
			getline(&buffer, &len, curlevel);
			debug("LEVEL%d: %s", c, buffer);
			for (x = 0; x < w; x++) {
				cur = buffer[x];
				switch (cur) {
					case '\n':
						break;
					case 'C':
						warp_player(x*BLOCK_DIM, y*BLOCK_DIM);
					case ' ':
						LEVELS[c]->dimensions[x][y] = FLOOR;
						break;
					case 'S':
						spawn_enemy(x*BLOCK_DIM, y*BLOCK_DIM,
								32, 32, 0, 10, 8, 2, 10);
						LEVELS[c]->dimensions[x][y] = FLOOR;
						break;
					case '#':
						LEVELS[c]->dimensions[x][y] = WALL;
						break;
					default:
						log_err("Invalid syntax in level%d: %c", c, cur);
				}
			}
		}
	}
}

void load_level(int index)
{
	CURRENT_LEVEL = LEVELS[index];
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
