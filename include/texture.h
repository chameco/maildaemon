#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <libguile.h>

#include "utils.h"

#define SPRITESHEET_DIM 16

typedef struct texture {
	GLuint texture;
	int w, h;
	int row_count, column_count;
	int anim_x, anim_y;
	GLuint index_handlers[SPRITESHEET_DIM * SPRITESHEET_DIM];
	GLuint vertex_handler;
} texture;

GLuint surface_to_texture(SDL_Surface *s);

void initialize_texture();

scm_t_bits get_texture_tag();
texture *load_texture(char *path, int w, int h);
void draw_texture(texture *r, double x, double y);
void draw_texture_scale(texture *r, double x, double y, int w, int h);
void draw_texture_scale_rotate(texture *r, double x, double y, int w, int h, double rotation);
int get_sheet_row(texture *r);
void set_sheet_row(texture *r, int ay);
int get_sheet_column(texture *r);
void set_sheet_column(texture *r, int ax);
void free_texture(texture *r);
