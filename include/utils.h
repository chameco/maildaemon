#pragma once

#include <stdbool.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "cuttle/utils.h"

typedef enum direction {
	NORTH=0,
	SOUTH=1,
	WEST=2,
	EAST=3
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
