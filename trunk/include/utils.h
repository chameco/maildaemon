#ifndef UTILS_H
#define UTILS_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "debug.h"
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

typedef struct list_node {
	struct list_node *prev;
	struct list_node *next;
	void *data;
} list_node;

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
list_node *make_node(void *d);
list_node *make_list();
void insert_list(list_node *l, void *data);
void remove_list(list_node *l, void *data);
GLuint surface_to_texture(SDL_Surface *s);
GLuint load_texture(char *file);
GLuint load_text(TTF_Font *font, char *text, int *w, int *h);
int check_collision(SDL_Rect A, SDL_Rect B);
double calculate_distance(double x1, double y1, double x2, double y2);
void mouse_coords(int x, int y, GLdouble *ox, GLdouble *oy);
int gcd(int ak, int am);
#endif
