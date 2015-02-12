#pragma once

#include "utils.h"

typedef struct line {
	line_point p1;
	line_point p2;
} line;

typedef struct ray {
	line l;
	double theta;
} ray;

typedef struct triangle {
	line_point p1;
	line_point p2;
	line_point p3;
} triangle;

void initialize_sight();
void reset_sight();
void find_corners();
void update_sight();
void draw_sight();
