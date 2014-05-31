#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "utils.h"

#define SPRITESHEET_DIM 4

typedef struct texture {
	GLuint texture;
	int w, h;
	int anim_x, anim_y;
	GLuint index_handlers[SPRITESHEET_DIM * SPRITESHEET_DIM];
	GLuint vertex_handler;
} texture;

GLuint surface_to_texture(SDL_Surface *s);

void initialize_texture();

texture *load_texture(char *path, int w, int h);
void draw_texture(texture *r, int x, int y);
void draw_texture_scale(texture *r, int x, int y, int w, int h);
int get_sheet_row(texture *r);
void set_sheet_row(texture *r, int ay);
int get_sheet_column(texture *r);
void set_sheet_column(texture *r, int ax);
void free_texture(texture *r);
