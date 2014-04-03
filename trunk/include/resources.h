#ifndef PURGE_RESOURCES_H
#define PURGE_RESOURCES_H
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "utils.h"

typedef struct resource {
	GLuint texture;
	int w, h;
} resource;

GLuint surface_to_texture(SDL_Surface *s);

void initialize_resources();

resource *load_resource(char *path);
void draw_resource(resource *r, int x, int y);
void draw_resource_scale(resource *r, int x, int y, int w, int h);
void free_resource(resource *r);
#endif
