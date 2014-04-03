#ifndef PURGE_UTILS_H
#define PURGE_UTILS_H
#include <GL/glew.h>
#include <SDL2/SDL.h>
#ifndef DATADIR
#define DATADIR ""
#endif

typedef enum direction {
	NORTH,
	SOUTH,
	WEST,
	EAST
} direction;

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
GLuint get_standard_vertices_handler();
int check_collision(SDL_Rect A, SDL_Rect B);
#endif
