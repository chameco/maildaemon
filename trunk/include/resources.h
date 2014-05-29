#ifndef PURGE_RESOURCES_H
#define PURGE_RESOURCES_H
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "utils.h"

#define SPRITESHEET_DIM 4

typedef struct resource {
	GLuint texture;
	int w, h;
	int anim_x, anim_y;
	GLuint index_handlers[SPRITESHEET_DIM * SPRITESHEET_DIM];
	GLuint vertex_handler;
} resource;

GLuint surface_to_texture(SDL_Surface *s);

void initialize_resources();

resource *load_resource(char *path, int w, int h);
void draw_resource(resource *r, int x, int y);
void draw_resource_scale(resource *r, int x, int y, int w, int h);
void set_animation(resource *r, int ay);
int get_frame(resource *r);
void set_frame(resource *r, int ax);
void free_resource(resource *r);
#endif
