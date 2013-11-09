#include "resources.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"

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

resource *load_resource(char *path)
{
	resource *ret = (resource *) malloc(sizeof(resource));
	SDL_Surface *surface = IMG_Load(path);
	char buffer[100];
	if (surface == NULL) {
		strcpy(buffer, DATADIR);
		strcat(buffer, path);
		surface = IMG_Load(buffer);
	}
	if (surface == NULL) {
		log_err("File %s does not exist", path);
		exit(1);
	}
	int w = surface->w;
	int h = surface->h;
	ret->texture = surface_to_texture(surface);
	ret->vertices[0].x = 0;
	ret->vertices[0].y = 0;

	ret->vertices[1].x = w;
	ret->vertices[1].y = 0;

	ret->vertices[2].x = w;
	ret->vertices[2].y = h;

	ret->vertices[3].x = 0;
	ret->vertices[3].y = h;

	ret->vertices[0].s = 0;
	ret->vertices[0].t = 0;

	ret->vertices[1].s = 1;
	ret->vertices[1].t = 0;

	ret->vertices[2].s = 1;
	ret->vertices[2].t = 1;

	ret->vertices[3].s = 0;
	ret->vertices[3].t = 1;

	glGenBuffers(1, &ret->vertex_handler);
	glBindBuffer(GL_ARRAY_BUFFER, ret->vertex_handler);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), ret->vertices, GL_STATIC_DRAW);
	return ret;
}

void draw_resource(resource *r, int x, int y)
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, r->texture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, r->vertex_handler);
		glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
		glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void free_resource(resource *r)
{
	glDeleteBuffers(1, &r->vertex_handler);
	free(r);
}
