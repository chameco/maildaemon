#ifndef PURGE_RESOURCES_H
#define PURGE_RESOURCES_H
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "utils.h"

typedef struct resource {
	GLuint texture;
	vertex vertices[4];
	GLuint vertex_handler;
} resource;

typedef struct sound {
	Mix_Chunk *sound;
} sound;

GLuint surface_to_texture(SDL_Surface *s);

resource *load_resource(char *path);
void draw_resource(resource *r, int x, int y);
void free_resource(resource *r);
#endif
