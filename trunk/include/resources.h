#ifndef PURGE_RESOURCES_H
#define PURGE_RESOURCES_H
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "utils.h"
#ifndef DATADIR
#define DATADIR ""
#endif

typedef struct resource {
	GLuint texture;
	vertex vertices[4];
	GLuint vertex_handler;
} resource;

GLuint surface_to_texture(SDL_Surface *s);

resource *load_resource(char *path);
void draw_resource(resource *r, int x, int y);
void free_resource(resource *r);
#endif
