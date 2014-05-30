#include "texture.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
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

SCM __api_load_texture(SCM path, SCM w, SCM h)
{
	char *p = scm_to_locale_string(path);
	texture *r = load_texture(p, scm_to_int(w), scm_to_int(h));
	free(p);
	return scm_new_smob(__api_texture_tag, (unsigned long) r);
}

SCM __api_draw_texture(SCM r, SCM x, SCM y)
{
	texture *res = (texture *) SCM_SMOB_DATA(r);
	draw_texture(res, scm_to_int(x), scm_to_int(y));
	return SCM_BOOL_F;
}

SCM __api_set_animation(SCM r, SCM ay)
{
	texture *res = (texture *) SCM_SMOB_DATA(r);
	set_animation(res, scm_to_int(ay));
	return SCM_BOOL_F;
}

SCM __api_get_frame(SCM r)
{
	texture *res = (texture *) SCM_SMOB_DATA(r);
	return scm_from_int(get_frame(res));
}

SCM __api_set_frame(SCM r, SCM ax)
{
	texture *res = (texture *) SCM_SMOB_DATA(r);
	set_frame(res, scm_to_int(ax));
	return SCM_BOOL_F;
}

void initialize_texture()
{
	TEXTURE_CACHE = make_hash_map();

	__api_texture_tag = scm_make_smob_type("texture", sizeof(texture));
	scm_c_define_gsubr("load-texture", 3, 0, 0, __api_load_texture);
	scm_c_define_gsubr("draw-texture", 3, 0, 0, __api_draw_texture);
	scm_c_define_gsubr("set-animation", 2, 0, 0, __api_set_animation);
	scm_c_define_gsubr("get-frame", 1, 0, 0, __api_get_frame);
	scm_c_define_gsubr("set-frame", 2, 0, 0, __api_set_frame);
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
	texture *ret = scm_gc_malloc(sizeof(texture), "texture");
	if (get_hash(TEXTURE_CACHE, path) == NULL) {
		SDL_Surface *surface = IMG_Load(path);
		if (surface == NULL) {
			log_err("File %s does not exist", path);
			exit(1);
		}
		ret->texture = surface_to_texture(surface);
		ret->w = w == 0 ? surface->w : w;
		ret->h = h == 0 ? surface->h : h;
		ret->anim_x = ret->anim_y = 0;

		GLuint index_data[4];
		vertex vertex_data[64];
		int row_count = surface->h / ret->h;
		int column_count = surface->w / ret->w;

		glGenBuffers(1, &(ret->vertex_handler));
		glGenBuffers(row_count * column_count, ret->index_handlers);

		int i = 0;
		for (int rc = 0; rc < row_count; ++rc) {
			for (int cc = 0; cc < column_count; ++cc) {
				index_data[0] = i * 4 + 0;
				index_data[1] = i * 4 + 1;
				index_data[2] = i * 4 + 2;
				index_data[3] = i * 4 + 3;

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret->index_handlers[i]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), index_data, GL_STATIC_DRAW);

				vertex_data[index_data[0]].x = cc;
				vertex_data[index_data[0]].y = rc;
				vertex_data[index_data[0]].s = ((GLfloat) cc)/column_count;
				vertex_data[index_data[0]].t = ((GLfloat) rc)/row_count;

				vertex_data[index_data[1]].x = cc + 1;
				vertex_data[index_data[1]].y = rc;
				vertex_data[index_data[1]].s = ((GLfloat) (cc + 1))/column_count;
				vertex_data[index_data[1]].t = ((GLfloat) rc)/row_count;

				vertex_data[index_data[2]].x = cc + 1;
				vertex_data[index_data[2]].y = rc + 1;
				vertex_data[index_data[2]].s = ((GLfloat) (cc + 1))/column_count;
				vertex_data[index_data[2]].t = ((GLfloat) (rc + 1))/row_count;

				vertex_data[index_data[3]].x = cc;
				vertex_data[index_data[3]].y = rc + 1;
				vertex_data[index_data[3]].s = ((GLfloat) cc)/column_count;
				vertex_data[index_data[3]].t = ((GLfloat) (rc + 1))/row_count;

				++i;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, ret->vertex_handler);
		glBufferData(GL_ARRAY_BUFFER, row_count * column_count * 4 * sizeof(vertex), vertex_data, GL_STATIC_DRAW);

		set_hash(TEXTURE_CACHE, path, (void *) ret);

		return ret;
	} else {
		texture *ret = scm_gc_malloc(sizeof(texture), "texture");
		memcpy(ret, (texture *) get_hash(TEXTURE_CACHE, path), sizeof(texture));
		return ret;
	}
}

void draw_texture(texture *r, int x, int y)
{
	draw_texture_scale(r, x, y, r->w, r->h);
}

void draw_texture_scale(texture *r, int x, int y, int w, int h)
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(w, h, 1);

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, r->texture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, r->vertex_handler);
	//glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)((sizeof(vertex) * r->anim_x * r->anim_y) + (sizeof(GLfloat)*2)));
	//glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(vertex) * r->anim_x * r->anim_y));	

	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0x0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *((r->index_handlers) + (r->anim_y * SPRITESHEET_DIM) + (r->anim_x)));
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void set_animation(texture *r, int ay)
{
	r->anim_y = ay;
}

int get_frame(texture *r)
{
	return r->anim_x;
}

void set_frame(texture *r, int ax)
{
	r->anim_x = ax;
}

void free_texture(texture *r)
{
	scm_gc_free(r, sizeof(texture), "texture");
}
