#ifndef PURGE_UTILS_H
#define PURGE_UTILS_H
#include <GL/glew.h>
#include <SDL2/SDL.h>
#ifndef DATADIR
#define DATADIR ""
#endif

typedef enum mode {
	TITLE_MODE,
	MAIN_MENU_MODE,
	DRAW_MODE,
	GAME_OVER_MODE
} mode;

typedef enum direction {
	NORTH,
	SOUTH,
	WEST,
	EAST
} direction;

typedef enum etype {
	EXPLOSION,
	SMOKE_CONST,
	SMOKE
} etype;

typedef struct vertex {
	GLfloat x, y;
	GLfloat s, t;
} vertex;

typedef struct color {
	GLfloat r, g, b, a;
} color;

color COLOR_RED;
color COLOR_GREEN;
color COLOR_BLUE;
color COLOR_WHITE;
color COLOR_BLACK;
color COLOR_GRAY;

void initialize_utils();
GLuint get_standard_indices_handler();
int check_collision(SDL_Rect A, SDL_Rect B);
double calculate_distance(double x1, double y1, double x2, double y2);
void mouse_coords(int x, int y, GLdouble *ox, GLdouble *oy);
int gcd(int ak, int am);
#endif
