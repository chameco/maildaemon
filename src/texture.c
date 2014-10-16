#include "texture.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <solid/solid.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"

static hash_map *TEXTURE_CACHE;

void __api_load_texture(solid_vm *vm)
{
	int h = solid_get_int_value(solid_pop_stack(vm));
	int w = solid_get_int_value(solid_pop_stack(vm));
	char *p = solid_get_str_value(solid_pop_stack(vm));
	texture *r = load_texture(p, w, h);
	vm->regs[255] = solid_struct(vm, r);
}

void __api_draw_texture(solid_vm *vm)
{
	double y = solid_get_double_value(solid_pop_stack(vm));
	double x = solid_get_double_value(solid_pop_stack(vm));
	texture *t = solid_get_struct_value(solid_pop_stack(vm));
	draw_texture(t, x, y);
}

void __api_get_sheet_row(solid_vm *vm)
{
	texture *t = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_int(vm, get_sheet_row(t));
}

void __api_set_sheet_row(solid_vm *vm)
{
	int ay = solid_get_int_value(solid_pop_stack(vm));
	texture *t = solid_get_struct_value(solid_pop_stack(vm));
	set_sheet_row(t, ay);
}

void __api_get_sheet_column(solid_vm *vm)
{
	texture *t = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_int(vm, get_sheet_column(t));
}

void __api_set_sheet_column(solid_vm *vm)
{
	int ax = solid_get_int_value(solid_pop_stack(vm));
	texture *t = solid_get_struct_value(solid_pop_stack(vm));
	set_sheet_column(t, ax);
}

void initialize_texture()
{
	TEXTURE_CACHE = make_hash_map();

	defun("load_texture", __api_load_texture);
	defun("draw_texture", __api_draw_texture);
	defun("get_sheet_row", __api_get_sheet_row);
	defun("set_sheet_row", __api_set_sheet_row);
	defun("get_sheet_column", __api_get_sheet_column);
	defun("set_sheet_column", __api_set_sheet_column);
}

GLuint surface_to_texture(SDL_Surface *surface)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int mode;
	if (surface->format->BytesPerPixel == 4) {
		if (surface->format->Rmask == 0x000000ff) {
			mode = GL_RGBA;
		} else {
			mode = GL_BGRA;
		}
	}
	else {
		if (surface->format->Rmask == 0x000000ff) {
			mode = GL_RGB;
		} else {
			mode = GL_BGR;
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0,
			mode, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);
	return texture;
}

texture *load_texture(char *path, int w, int h)
{
	if (get_hash(TEXTURE_CACHE, path) == NULL) {
		texture *cached = (texture *) malloc(sizeof(texture));
		SDL_Surface *surface = IMG_Load(path);
		if (surface == NULL) {
			return NULL;
		}
		cached->texture = surface_to_texture(surface);
		cached->w = (w == 0) ? surface->w : w;
		cached->h = (h == 0) ? surface->h : h;
		cached->anim_x = cached->anim_y = 0;

		GLuint index_data[4];
		vertex vertex_data[4 * SPRITESHEET_DIM * SPRITESHEET_DIM];
		cached->row_count = surface->h / cached->h;
		cached->column_count = surface->w / cached->w;

		glGenBuffers(1, &(cached->vertex_handler));
		glGenBuffers(cached->row_count * cached->column_count, cached->index_handlers);

		int i = 0;
		for (int rc = 0; rc < cached->row_count; ++rc) {
			for (int cc = 0; cc < cached->column_count; ++cc) {
				index_data[0] = i * 4 + 0;
				index_data[1] = i * 4 + 1;
				index_data[2] = i * 4 + 2;
				index_data[3] = i * 4 + 3;

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cached->index_handlers[i]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), index_data, GL_STATIC_DRAW);

				vertex_data[index_data[0]].x = cc;
				vertex_data[index_data[0]].y = rc;
				vertex_data[index_data[0]].s = ((GLfloat) cc)/cached->column_count;
				vertex_data[index_data[0]].t = ((GLfloat) rc)/cached->row_count;

				vertex_data[index_data[1]].x = cc + 1;
				vertex_data[index_data[1]].y = rc;
				vertex_data[index_data[1]].s = ((GLfloat) (cc + 1))/cached->column_count;
				vertex_data[index_data[1]].t = ((GLfloat) rc)/cached->row_count;

				vertex_data[index_data[2]].x = cc + 1;
				vertex_data[index_data[2]].y = rc + 1;
				vertex_data[index_data[2]].s = ((GLfloat) (cc + 1))/cached->column_count;
				vertex_data[index_data[2]].t = ((GLfloat) (rc + 1))/cached->row_count;

				vertex_data[index_data[3]].x = cc;
				vertex_data[index_data[3]].y = rc + 1;
				vertex_data[index_data[3]].s = ((GLfloat) cc)/cached->column_count;
				vertex_data[index_data[3]].t = ((GLfloat) (rc + 1))/cached->row_count;

				++i;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, cached->vertex_handler);
		glBufferData(GL_ARRAY_BUFFER, cached->row_count * cached->column_count * 4 * sizeof(vertex), vertex_data, GL_STATIC_DRAW);

		set_hash(TEXTURE_CACHE, path, (void *) cached);
	}

	texture *ret = malloc(sizeof(texture));
	memcpy(ret, (texture *) get_hash(TEXTURE_CACHE, path), sizeof(texture));
	return ret;
}

void draw_texture(texture *r, double x, double y)
{
	draw_texture_scale(r, x, y, r->w, r->h);
}

void draw_texture_scale(texture *r, double x, double y, int w, int h)
{
	//draw_texture_scale_rotate(r, x, y, r->w, r->h, 0.0);
	glPushMatrix();
	glTranslatef(x-(r->anim_x * w), y-(r->anim_y * h), 0);
	glScalef(w, h, 1);

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, r->texture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, r->vertex_handler);

	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0x0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *((r->index_handlers) + (r->anim_y * r->column_count) + (r->anim_x)));
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void draw_texture_scale_rotate(texture *r, double x, double y, int w, int h, double rotation)
{
	glPushMatrix();
	glTranslatef(x-(r->anim_x * w), y-(r->anim_y * h), 0);
	glScalef(w, h, 1);
	glRotatef((rotation * 180)/3.141592654, 0.0, 0.0, 1.0);

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, r->texture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, r->vertex_handler);

	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0x0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *((r->index_handlers) + (r->anim_y * r->column_count) + (r->anim_x)));
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

int get_sheet_row(texture *r)
{
	return r->anim_y;
}

void set_sheet_row(texture *r, int ay)
{
	r->anim_y = ay;
}

int get_sheet_column(texture *r)
{
	return r->anim_x;
}

void set_sheet_column(texture *r, int ax)
{
	r->anim_x = ax;
}

void free_texture(texture *r)
{
	free(r);
}
