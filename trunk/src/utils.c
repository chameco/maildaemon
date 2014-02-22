#include "utils.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL2/SDL_image.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "vm.h"

GLuint STANDARD_INDICES[4] = {0, 1, 2, 3};
GLuint STANDARD_INDICES_HANDLER = 0;

color COLOR_RED = {1.0, 0.0, 0.0, 1.0};
color COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
color COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
color COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
color COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
color COLOR_GRAY = {0.1, 0.1, 0.1, 1.0};

PAPI_field_from_struct(solid_double, vertex, x)
PAPI_field_from_struct(solid_double, vertex, y)
PAPI_field_from_struct(solid_double, vertex, s)
PAPI_field_from_struct(solid_double, vertex, t)

PAPI_field_from_struct(solid_double, color, r)
PAPI_field_from_struct(solid_double, color, g)
PAPI_field_from_struct(solid_double, color, b)
PAPI_field_from_struct(solid_double, color, a)

PAPI_ptr_var(solid_struct, COLOR_RED)
PAPI_ptr_var(solid_struct, COLOR_GREEN)
PAPI_ptr_var(solid_struct, COLOR_BLUE)
PAPI_ptr_var(solid_struct, COLOR_WHITE)
PAPI_ptr_var(solid_struct, COLOR_BLACK)
PAPI_ptr_var(solid_struct, COLOR_GRAY)

void initialize_utils_api()
{
	solid_object *ns = make_namespace("Utils");
	define_function(ns, "get_vertex_x", PAPI_vertex_x);
	define_function(ns, "get_vertex_y", PAPI_vertex_y);
	define_function(ns, "get_vertex_s", PAPI_vertex_s);
	define_function(ns, "get_vertex_t", PAPI_vertex_t);

	define_function(ns, "get_color_r", PAPI_color_r);
	define_function(ns, "get_color_g", PAPI_color_g);
	define_function(ns, "get_color_b", PAPI_color_b);
	define_function(ns, "get_color_a", PAPI_color_a);

	define_function(ns, "color_red", PAPI_COLOR_RED);
	define_function(ns, "color_green", PAPI_COLOR_GREEN);
	define_function(ns, "color_blue", PAPI_COLOR_BLUE);
	define_function(ns, "color_white", PAPI_COLOR_WHITE);
	define_function(ns, "color_black", PAPI_COLOR_BLACK);
	define_function(ns, "color_gray", PAPI_COLOR_GRAY);
}

void initialize_utils()
{
	glGenBuffers(1, &STANDARD_INDICES_HANDLER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, STANDARD_INDICES_HANDLER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), STANDARD_INDICES, GL_STATIC_DRAW);
	initialize_utils_api();
}

GLuint get_standard_indices_handler()
{
	return STANDARD_INDICES_HANDLER;
}

int check_collision(SDL_Rect A, SDL_Rect B)
{

	return !(((A.x >= B.x && A.x < B.x + B.w)  ||
				(B.x >= A.x && B.x < A.x + A.w)) && 
			((A.y >= B.y && A.y < B.y + B.h)   ||
			 (B.y >= A.y && B.y < A.y + A.h)));
}
