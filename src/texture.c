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
#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"

static hash_map *TEXTURE_CACHE;

static scm_t_bits __api_texture_tag;

scm_t_bits get_texture_tag()
{
	return __api_texture_tag;
}

SCM __api_load_texture(SCM path, SCM w, SCM h)
{
	char *p = scm_to_locale_string(path);
	texture *r = load_texture(p, scm_to_int(w), scm_to_int(h));
	free(p);
	SCM ret = scm_new_smob(__api_texture_tag, (unsigned long) r);
	return ret;
}

SCM __api_draw_texture(SCM r, SCM x, SCM y)
{
	texture *res = (texture *) SCM_SMOB_DATA(r);
	draw_texture(res, scm_to_double(x), scm_to_double(y));
	return SCM_BOOL_F;
}

SCM __api_get_sheet_row(SCM r)
{
	texture *res = (texture *) SCM_SMOB_DATA(r);
	return scm_from_int(get_sheet_row(res));
}

SCM __api_set_sheet_row(SCM r, SCM ay)
{
	texture *res = (texture *) SCM_SMOB_DATA(r);
	set_sheet_row(res, scm_to_int(ay));
	return SCM_BOOL_F;
}

SCM __api_get_sheet_column(SCM r)
{
	texture *res = (texture *) SCM_SMOB_DATA(r);
	return scm_from_int(get_sheet_column(res));
}

SCM __api_set_sheet_column(SCM r, SCM ax)
{
	texture *res = (texture *) SCM_SMOB_DATA(r);
	set_sheet_column(res, scm_to_int(ax));
	return SCM_BOOL_F;
}

void initialize_texture()
{
	TEXTURE_CACHE = make_hash_map();

	__api_texture_tag = scm_make_smob_type("texture", sizeof(texture));
	scm_c_define_gsubr("load-texture", 3, 0, 0, __api_load_texture);
	scm_c_define_gsubr("draw-texture", 3, 0, 0, __api_draw_texture);
	scm_c_define_gsubr("get-sheet-row", 1, 0, 0, __api_get_sheet_row);
	scm_c_define_gsubr("set-sheet-row", 2, 0, 0, __api_set_sheet_row);
	scm_c_define_gsubr("get-sheet-column", 1, 0, 0, __api_get_sheet_column);
	scm_c_define_gsubr("set-sheet-column", 2, 0, 0, __api_set_sheet_column);
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
			log_err("File %s does not exist", path);
			exit(1);
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
