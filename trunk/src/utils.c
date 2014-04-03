#include "utils.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL2/SDL_image.h>
#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

color COLOR_RED = {1.0, 0.0, 0.0, 1.0};
color COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
color COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
color COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
color COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
color COLOR_GRAY = {0.1, 0.1, 0.1, 1.0};

static GLuint STANDARD_INDICES[4] = {0, 1, 2, 3};
static GLuint STANDARD_INDICES_HANDLER = 0;

static vertex STANDARD_VERTICES[4] = {
	{0, 0, 0, 0},
	{1, 0, 1, 0},
	{1, 1, 1, 1},
	{0, 1, 0, 1}
};
static GLuint STANDARD_VERTICES_HANDLER = 0;

static scm_t_bits __api_color_tag;

SCM __api_make_color(SCM r, SCM g, SCM b, SCM a)
{
	color *c = scm_gc_malloc(sizeof(color), "color");
	c->r = scm_to_double(r);
	c->g = scm_to_double(g);
	c->b = scm_to_double(b);
	c->a = scm_to_double(a);
	return scm_new_smob(__api_color_tag, (unsigned long) c);
}

void initialize_utils()
{
	glGenBuffers(1, &STANDARD_INDICES_HANDLER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, STANDARD_INDICES_HANDLER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), STANDARD_INDICES, GL_STATIC_DRAW);

	glGenBuffers(1, &STANDARD_VERTICES_HANDLER);
	glBindBuffer(GL_ARRAY_BUFFER, STANDARD_VERTICES_HANDLER);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), STANDARD_VERTICES, GL_STATIC_DRAW);

	__api_color_tag = scm_make_smob_type("color", sizeof(color));
	scm_c_define_gsubr("make-color", 4, 0, 0, __api_make_color);
}

GLuint get_standard_indices_handler()
{
	return STANDARD_INDICES_HANDLER;
}

GLuint get_standard_vertices_handler()
{
	return STANDARD_VERTICES_HANDLER;
}

int check_collision(SDL_Rect A, SDL_Rect B)
{

	return !(((A.x >= B.x && A.x < B.x + B.w)  ||
				(B.x >= A.x && B.x < A.x + A.w)) && 
			((A.y >= B.y && A.y < B.y + B.h)   ||
			 (B.y >= A.y && B.y < A.y + A.h)));
}
