#pragma once

#include <stdbool.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <libguile.h>

#include "cuttle/utils.h"

typedef enum direction {
	DIR_NORTH=0,
	DIR_SOUTH=1,
	DIR_WEST=2,
	DIR_EAST=3
} direction;

typedef struct vertex {
	GLfloat x, y;
	GLfloat s, t;
} vertex;

typedef struct line_point {
	double x, y;
} line_point;

typedef struct color {
	GLfloat r, g, b, a;
} color;

typedef enum thunk_type {
	THUNK_NULL,
	THUNK_C,
	THUNK_SCM
} thunk_type;

typedef union thunk_data {
	void (*cfunc)();
	SCM scmfunc;
} thunk_data;

typedef struct thunk {
	thunk_type type;
	thunk_data data;
} thunk;

color COLOR_RED;
color COLOR_GREEN;
color COLOR_BLUE;
color COLOR_WHITE;
color COLOR_BLACK;
color COLOR_GRAY;

void initialize_utils();

GLuint get_standard_indices_handler();
GLuint get_standard_vertices_handler();

bool check_collision(SDL_Rect A, SDL_Rect B);
double calculate_angle(double originx, double originy, double pointx, double pointy);
line_point *bresenham_line(double x0, double y0, double x1, double y1);
bool is_unbroken_line(double x0, double y0, double x1, double y1);

thunk make_thunk(void (*cfunc)());
thunk make_thunk_scm(SCM scmfunc);
void execute_thunk(thunk cb);
