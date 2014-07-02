#pragma once

#include "utils.h"

typedef struct lightsource {
	double x, y;
	int dim, intensity;
	color c;
} lightsource;

void initialize_lightsource();

void reset_lightsource();

lightsource *make_lightsource(double x, double y, int dim, int intensity, color c);
void spawn_lightsource(lightsource *l);

void draw_lightsource();
