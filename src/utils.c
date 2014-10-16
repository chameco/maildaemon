#include "utils.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <math.h>

#include <SDL2/SDL_image.h>
#include <solid/solid.h>

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

static solid_vm *VM;

void __api_make_color(solid_vm *vm)
{
	color *c = malloc(sizeof(color));
	c->a = solid_get_double_value(solid_pop_stack(vm));
	c->b = solid_get_double_value(solid_pop_stack(vm));
	c->g = solid_get_double_value(solid_pop_stack(vm));
	c->r = solid_get_double_value(solid_pop_stack(vm));
	vm->regs[255] = solid_struct(vm, c);
}

void __api_get_tile_dim(solid_vm *vm)
{
	vm->regs[255] = solid_int(vm, TILE_DIM);
}

void __api_calculate_angle(solid_vm *vm)
{
	double py = solid_get_double_value(solid_pop_stack(vm));
	double px = solid_get_double_value(solid_pop_stack(vm));
	double oy = solid_get_double_value(solid_pop_stack(vm));
	double ox = solid_get_double_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, calculate_angle(ox, oy, px, py));
}

void __api_is_unbroken_line(solid_vm *vm)
{
	double y1 = solid_get_double_value(solid_pop_stack(vm));
	double x1 = solid_get_double_value(solid_pop_stack(vm));
	double y0 = solid_get_double_value(solid_pop_stack(vm));
	double x0 = solid_get_double_value(solid_pop_stack(vm));
	vm->regs[255] = solid_bool(vm, is_unbroken_line(x0, y0, x1, y1));
}

void __api_rand(solid_vm *vm)
{
	vm->regs[255] = solid_int(vm, rand());
}

void initialize_utils()
{
	VM = solid_make_vm();

	glGenBuffers(1, &STANDARD_INDICES_HANDLER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, STANDARD_INDICES_HANDLER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), STANDARD_INDICES, GL_STATIC_DRAW);

	glGenBuffers(1, &STANDARD_VERTICES_HANDLER);
	glBindBuffer(GL_ARRAY_BUFFER, STANDARD_VERTICES_HANDLER);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), STANDARD_VERTICES, GL_STATIC_DRAW);

	defun("make_color", __api_make_color);
	defun("get_tile_dim", __api_get_tile_dim);
	defun("calculate_angle", __api_calculate_angle);
	defun("is_unbroken_line", __api_is_unbroken_line);
	defun("random_integer", __api_rand);
}

solid_vm *get_vm()
{
	return VM;
}

void defun(char *s, void (*function)(solid_vm *vm))
{
	solid_vm *vm = get_vm();
	solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, s), solid_define_c_function(vm, function));
}

GLuint get_standard_indices_handler()
{
	return STANDARD_INDICES_HANDLER;
}

GLuint get_standard_vertices_handler()
{
	return STANDARD_VERTICES_HANDLER;
}

solid_object *load(char *path)
{
	solid_call_func(get_vm(), solid_parse_tree(get_vm(), solid_parse_file(path)));
	return get_vm()->regs[255];
}

void load_all(char *dir)
{
	char buf[256];
	DIR *d = opendir(dir);
	struct dirent *entry;
	if (d != NULL) {
		while ((entry = readdir(d))) {
			char *pos = strrchr(entry->d_name, '.');
			if (pos != NULL && strcmp(pos + 1, "sol") == 0) {
				sprintf(buf, "%s/%s", dir, entry->d_name);
				load(buf);
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

thunk make_thunk_solid(solid_object *o)
{
	thunk ret;
	if (o->type == T_FUNC || o->type == T_CFUNC) {
		ret.type = THUNK_SOLID;
		ret.data.solfunc = o;
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
		case THUNK_SOLID:
			solid_call_func(get_vm(), cb.data.solfunc);
			break;
		default:
			log_err("Invalid callback type");
			exit(1);
			break;
	}
}
