#include "utils.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <math.h>

#include <SDL2/SDL_image.h>
#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "level.h"

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
	color *c = malloc(sizeof(color));
	c->r = scm_to_double(r);
	c->g = scm_to_double(g);
	c->b = scm_to_double(b);
	c->a = scm_to_double(a);
	return scm_new_smob(__api_color_tag, (unsigned long) c);
}

SCM __api_get_tile_dim()
{
	return scm_from_int(TILE_DIM);
}

SCM __api_calculate_angle(SCM ox, SCM oy, SCM px, SCM py)
{
	return scm_from_double(calculate_angle(scm_to_double(ox), scm_to_double(oy), scm_to_double(px), scm_to_double(py)));
}

SCM __api_is_unbroken_line(SCM x0, SCM y0, SCM x1, SCM y1)
{
	return scm_from_bool(is_unbroken_line(scm_to_double(x0), scm_to_double(y0), scm_to_double(x1), scm_to_double(y1)));
}

SCM __api_rand()
{
	return scm_from_int(rand());
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
	scm_c_define_gsubr("get-tile-dim", 0, 0, 0, __api_get_tile_dim);
	scm_c_define_gsubr("calculate-angle", 4, 0, 0, __api_calculate_angle);
	scm_c_define_gsubr("is-unbroken-line", 4, 0, 0, __api_is_unbroken_line);
	scm_c_define_gsubr("random-integer", 0, 0, 0, __api_rand);
}

GLuint get_standard_indices_handler()
{
	return STANDARD_INDICES_HANDLER;
}

GLuint get_standard_vertices_handler()
{
	return STANDARD_VERTICES_HANDLER;
}

void load_all(char *dir)
{
	char buf[256];
	DIR *d = opendir(dir);
	struct dirent *entry;
	if (d != NULL) {
		while ((entry = readdir(d))) {
			char *pos = strrchr(entry->d_name, '.');
			if (pos != NULL && strcmp(pos + 1, "scm") == 0) {
				sprintf(buf, "%s/%s", dir, entry->d_name);
				scm_c_primitive_load(buf);
			}
		}
		closedir(d);
	} else {
		log_err("Directory \"%s\" does not exist", dir);
		exit(1);
	}
}

bool check_collision(SDL_Rect A, SDL_Rect B)
{

	return !(((A.x >= B.x && A.x < B.x + B.w)  ||
				(B.x >= A.x && B.x < A.x + A.w)) && 
			((A.y >= B.y && A.y < B.y + B.h)   ||
			 (B.y >= A.y && B.y < A.y + A.h)));
}

double calculate_angle(double originx, double originy, double pointx, double pointy)
{
	double xdiff = pointx - originx;
	double ydiff = pointy - originy;
	double slope;
	double angle;
	if (xdiff == 0) {
		if (ydiff > 0) {
			slope = INFINITY;
		} else {
			slope = -INFINITY;
		}
	} else {
		slope = ydiff/xdiff;
	}
	angle = atan(slope);
	if (xdiff < 0) {
		angle += 3.141592654;
	}
	return angle;
}

line_point *bresenham_line(double x0, double y0, double x1, double y1)
{
	int temp;
	if (x1 < x0) {
		temp = x0;
		x0 = x1;
		x1 = temp;
	}
	if (y1 < y0) {
		temp = y0;
		y0 = y1;
		y1 = temp;
	}
	double xdiff = x1 - x0;
	double ydiff = y1 - y0;
	line_point *ret;
	int c = 0;
	if (xdiff >= ydiff) {
		double error = 0;
		double delta_error = fabs(ydiff/xdiff);
		ret = (line_point *) calloc(fabs(xdiff) + 1, sizeof(line_point));
		double y = y0;
		for (double x = x0; x <= x1; ++x, ++c) {
			ret[c].x = x;
			ret[c].y = y;
			error += delta_error;
			if (error > 0.5) {
				++y;
				--error;
			}
		}
	} else {
		double error = 0;
		double delta_error = fabs(xdiff/ydiff);
		ret = (line_point *) calloc(fabs(ydiff) + 1, sizeof(line_point));
		double x = x0;
		for (double y = y0; y <= y1; ++y, ++c) {
			ret[c].x = x;
			ret[c].y = y;
			error += delta_error;
			if (error > 0.5) {
				++x;
				--error;
			}
		}
	}
	ret[c].x = -1;
	return ret;
}

bool is_unbroken_line(double x0, double y0, double x1, double y1)
{
	line_point *points = bresenham_line(x0, y0, x1, y1);
	for (int c = 0; points[c].x != -1; ++c) {
		if (is_solid_tile(points[c].x/32, points[c].y/32)) {
			free(points);
			return false;
		}
	}
	free(points);
	return true;
}

thunk make_thunk(void (*cfunc)())
{
	thunk ret;
	if (cfunc != NULL) {
		ret.type = THUNK_C;
		ret.data.cfunc = cfunc;
	} else {
		ret.type = THUNK_NULL;
	}
	return ret;
}

thunk make_thunk_scm(SCM scmfunc)
{
	thunk ret;
	if (scm_is_true(scmfunc)) {
		ret.type = THUNK_SCM;
		ret.data.scmfunc = scmfunc;
		scm_gc_protect_object(ret.data.scmfunc);
	} else {
		ret.type = THUNK_NULL;
	}
	return ret;
}

void execute_thunk(thunk cb)
{
	switch (cb.type) {
		case THUNK_NULL:
			break;
		case THUNK_C:
			cb.data.cfunc();
			break;
		case THUNK_SCM:
			scm_call_0(cb.data.scmfunc);
			break;
		default:
			log_err("Invalid callback type");
			exit(1);
			break;
	}
}

int max(int x, int y)
{
	return x > y ? x : y;
}
