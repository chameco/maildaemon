#ifndef UTILS_H
#define UTILS_H
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
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
GLuint load_texture(char *file);
int check_collision(SDL_Rect A, SDL_Rect B);
#endif
