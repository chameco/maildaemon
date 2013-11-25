#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cuttle/debug.h>
#include "utils.h"
#include "player.h"
#include "worldgen.h"
#include "entity.h"
#include "lights.h"
#include "fx.h"
#include "level.h"

region *room_from_file(char *path)
{
	int w, h;
	double ambience;
	FILE *f = fopen(path, "r");
	fscanf(f, "%d %d %lf\n", &w, &h, &ambience);
	region *r = generate_region("", w, h);
	r->ambience = ambience;
	int x = 0;
	int y = 0;
	entity *e;
	lightsource *l;
	effect *fx;
	while (y < h) {
		x = 0;
		while (x < w) {
			int c = fgetc(f);
			switch (c) {
				case '#':
					r->blocks[x][y] = STONE;
					x++;
					break;
				case ' ':
					r->blocks[x][y] = PLANKS;
					x++;
					break;
				case 'W':
					e = make_entity(1, x*get_block_dim(), y*get_block_dim(), 32, 32, NULL, 10, 4, 100.0);
					e->freeable = 0;
					r->entities[r->numentities++] = *e;
					free(e);
					r->blocks[x][y] = PLANKS;
					x++;
					break;
				case 'i':
					l = make_lightsource(x*get_block_dim() - 112, y*get_block_dim() - 112, 128, 3, COLOR_WHITE);
					l->freeable = 0;
					r->lights[r->numlights++] = *l;
					free(l);
					fx = make_fx(SMOKE_CONST, COLOR_GRAY, x*get_block_dim() + 16, y*get_block_dim(), 10, 50, 4);
					fx->freeable = 0;
					r->fx[r->numfx++] = *fx;
					free(fx);
					r->blocks[x][y] = TORCH;
					x++;
					break;
				case '\n':
					x = w;
					y++;
					break;
				default:
					r->blocks[x][y] = VOID;
					x++;
					break;
			}
		}
	}
	return r;
}

void make_world()
{
	world w;
	w.player_x = 0;
	w.player_y = 0;
	warp_player(160, 160);
	region *empty = room_from_file("rooms/empty");
	add_standard_room(empty);
	int x, y;
	for (x = 0; x < WORLD_DIM; ++x) {
		for (y = 0; y < WORLD_DIM; ++y) {
			w.regions[x][y] = generate_region("Name", 0, 0);
			populate_region(w.regions[x][y], 0);
		}
	}
	save_world(&w);
}

int main(int argc, char *argv[])
{
	make_world();
	return 0;
}
