#include "level.h"

level *CURRENT_LEVEL;
int CURRENT_LEVEL_INDEX;
char CURRENT_LEVEL_NAME[256];
double CURRENT_LEVEL_AMBIENCE;
GLuint BLOCK_TEXTURES[256];
const int LEVEL_COUNT = 0;
const int BLOCK_DIM = 32;
vertex BLOCK_VERTICES[4];
GLuint BLOCK_VERTEX_HANDLER = 0;

level *get_current_level()
{
	return CURRENT_LEVEL;
}

int get_current_level_index()
{
	return CURRENT_LEVEL_INDEX;
}

char *get_current_level_name()
{
	return CURRENT_LEVEL_NAME;
}

double get_current_level_ambience()
{
	return CURRENT_LEVEL_AMBIENCE;
}

int get_block_dim()
{
	return BLOCK_DIM;
}

int is_solid_block(block b)
{
	if (b == FLOOR) {
		return 0;
	} else {
		return 1;
	}
}

void initialize_levels()
{
	BLOCK_TEXTURES[VOID] = load_texture("textures/blank.png");
	BLOCK_TEXTURES[FLOOR] = load_texture("textures/floor.png");
	BLOCK_TEXTURES[WALL] = load_texture("textures/wall.png");
	BLOCK_TEXTURES[TORCH] = load_texture("textures/torch.png");

	BLOCK_VERTICES[0].x = 0;
	BLOCK_VERTICES[0].y = 0;

	BLOCK_VERTICES[1].x = BLOCK_DIM;
	BLOCK_VERTICES[1].y = 0;

	BLOCK_VERTICES[2].x = BLOCK_DIM;
	BLOCK_VERTICES[2].y = BLOCK_DIM;

	BLOCK_VERTICES[3].x = 0;
	BLOCK_VERTICES[3].y = BLOCK_DIM;

	BLOCK_VERTICES[0].s = 0;
	BLOCK_VERTICES[0].t = 0;

	BLOCK_VERTICES[1].s = 1;
	BLOCK_VERTICES[1].t = 0;

	BLOCK_VERTICES[2].s = 1;
	BLOCK_VERTICES[2].t = 1;

	BLOCK_VERTICES[3].s = 0;
	BLOCK_VERTICES[3].t = 1;

	glGenBuffers(1, &BLOCK_VERTEX_HANDLER);
	glBindBuffer(GL_ARRAY_BUFFER, BLOCK_VERTEX_HANDLER);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), BLOCK_VERTICES, GL_STATIC_DRAW);
}

void load_level(int index)
{
	int x, y, w, h;
	FILE *curlevel;
	char *buffer;
	size_t len = 0;
	char cur;
	char sprintf_fodder[100];
	reset_lights();
	reset_entities();
	reset_enemies();
	reset_projectile();
	reset_fx();
	CURRENT_LEVEL = (level *) malloc(sizeof(level));
	CURRENT_LEVEL_INDEX = index;
	memset((void *) CURRENT_LEVEL_NAME, 0, 256);
	x = y = w = h = 0;
	sprintf(sprintf_fodder, "levels/level%d", index);
	curlevel = fopen(sprintf_fodder, "r");
	if (curlevel == NULL) {
		sprintf(sprintf_fodder, "%slevels/level%d", DATADIR, index);
		debug("fallback level %s", sprintf_fodder);
		curlevel = fopen(sprintf_fodder, "r");
	}
	char *namebuf = NULL;
	size_t namesize = 256;
	getline(&namebuf, &namesize, curlevel);
	strncpy(CURRENT_LEVEL_NAME, namebuf, strlen(namebuf)-1);
	fscanf(curlevel, "%d %d %lf\n", &w, &h, &CURRENT_LEVEL_AMBIENCE);
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
				case '.':
					CURRENT_LEVEL->dimensions[x][y] = VOID;
					break;
				case 'S':
					SPAWN_SLIME(x*BLOCK_DIM, y*BLOCK_DIM);
					CURRENT_LEVEL->dimensions[x][y] = FLOOR;
					break;
				case 'W':
					SPAWN_WIZARD(x*BLOCK_DIM, y*BLOCK_DIM);
					CURRENT_LEVEL->dimensions[x][y] = FLOOR;
					break;
				case 'g':
					SPAWN_BOSSBLOB(x*BLOCK_DIM, y*BLOCK_DIM);
					CURRENT_LEVEL->dimensions[x][y] = FLOOR;
					break;
				case '0':
					SPAWN_TARGET(x*BLOCK_DIM, y*BLOCK_DIM);
					CURRENT_LEVEL->dimensions[x][y] = FLOOR;
					break;
				case '#':
					CURRENT_LEVEL->dimensions[x][y] = WALL;
					break;
				case '<':
					spawn_entity(0, x*BLOCK_DIM, y*BLOCK_DIM, 32, 32);
					CURRENT_LEVEL->dimensions[x][y] = FLOOR;
					spawn_lightsource((x * BLOCK_DIM + BLOCK_DIM / 2) - 64,
							(y * BLOCK_DIM + BLOCK_DIM / 2) - 64,
							128, 2, COLOR_GREEN);
					break;
				case '>':
					spawn_entity(1, x*BLOCK_DIM, y*BLOCK_DIM, 32, 32);
					CURRENT_LEVEL->dimensions[x][y] = FLOOR;
					spawn_lightsource((x * BLOCK_DIM + BLOCK_DIM / 2) - 64,
							(y * BLOCK_DIM + BLOCK_DIM / 2) - 64,
							128, 2, COLOR_RED);
					break;
				case 'i':
					CURRENT_LEVEL->dimensions[x][y] = TORCH;
					SPAWN_SMOKE_CONST(x * BLOCK_DIM + BLOCK_DIM / 2,
							y * BLOCK_DIM, 35);
					spawn_lightsource((x * BLOCK_DIM + BLOCK_DIM / 2) - 64,
							(y * BLOCK_DIM + BLOCK_DIM / 2) - 64,
							128, 2, COLOR_WHITE);
					break;
				default:
					log_err("Invalid syntax in level%d: %c", index, cur);
			}
		}
	}
	fclose(curlevel);
}

void draw_block(block b, int x, int y)
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glBindTexture(GL_TEXTURE_2D, BLOCK_TEXTURES[b]);

	glColor3f(1.0, 1.0, 1.0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, BLOCK_VERTEX_HANDLER);
		glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
		glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

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
