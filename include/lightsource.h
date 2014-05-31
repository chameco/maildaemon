#pragma once

#include "utils.h"

typedef struct lightsource {
	int x, y, dim, intensity;
	color c;
} lightsource;

void initialize_lightsource();

void reset_lightsource();

lightsource *make_lightsource(int x, int y, int dim, int intensity, color c);
void spawn_lightsource(lightsource *l);

void draw_lightsource();
