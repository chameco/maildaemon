#ifndef UTILS_H
#define UTILS_H
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
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
	DRAW_MODE,
	GAME_OVER_MODE
} mode;
typedef enum direction {
	NORTH,
	SOUTH,
	WEST,
	EAST
} direction;
typedef struct list_node {
	struct list_node *prev;
	struct list_node *next;
	void *data;
} list_node;
list_node *make_node(void *d);
list_node *make_list();
void insert_list(list_node *l, void *data);
void remove_list(list_node *l, void *data);
GLuint surface_to_texture(SDL_Surface *s);
GLuint load_texture(char *file);
GLuint load_text(TTF_Font *font, char *text, int *w, int *h);
int check_collision(SDL_Rect A, SDL_Rect B);
float calculate_distance(float x1, float y1, float x2, float y2);
#endif
