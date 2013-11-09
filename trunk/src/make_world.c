#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cuttle/debug.h>
#include "worldgen.h"

void make_world()
{
	world w;
	w.player_x = 0;
	w.player_y = 0;
	int x, y;
	register_standard_handlers();
	for (x = 0; x < WORLD_DIM; ++x) {
		for (y = 0; y < WORLD_DIM; ++y) {
			w.regions[x][y] = generate_region("Name", 10, 10);
		}
	}
	save_world(&w);
}

int main(int argc, char *argv[])
{
	make_world();
	return 0;
}
