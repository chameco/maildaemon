#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cuttle/debug.h>
#include "utils.h"
#include "player.h"
#include "worldgen.h"
#include "level.h"

region *room_from_file(char *path)
{
	int w, h;
	double ambience;
	int torch_intensity;
	FILE *f = fopen(path, "r");
	fscanf(f, "%d %d %lf %d\n", &w, &h, &ambience, &torch_intensity);
	region *r = generate_region("", w, h);
	r->ambience = ambience;
	int x = 0;
	int y = 0;
	char sprintf_fodder[256];
	while (y < h) {
		x = 0;
		while (x < w) {
			int c = fgetc(f);
			switch (c) {
				case ' ':
					r->blocks[x][y] = VOID;
					x++;
					break;
				case '#':
					r->blocks[x][y] = STONE;
					x++;
					break;
				case '.':
					r->blocks[x][y] = PLANKS;
					x++;
					break;
				case 'W':
					sprintf(sprintf_fodder, "Scripts.Entity.Wizard.make(%d, %d);", x*get_block_dim(), y*get_block_dim());
					strcpy(r->entities[r->numentities++], sprintf_fodder);
					r->blocks[x][y] = PLANKS;
					x++;
					break;
				case 'i':
					sprintf(sprintf_fodder, "Lights.make(%d, %d, 128, %d, Utils.color_white());", x*get_block_dim() - 112, y*get_block_dim() - 112, torch_intensity);
					strcpy(r->lights[r->numlights++], sprintf_fodder);
					sprintf(sprintf_fodder, "Fx.make(1, Utils.color_gray(), %d, %d, 10, 50, 4);", x*get_block_dim() + 16, y*get_block_dim());
					strcpy(r->fx[r->numfx++], sprintf_fodder);
					r->blocks[x][y] = TORCH;
					x++;
					break;
				case '\n':
					x = w;
					y++;
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
	add_standard_room(EMPTY, empty);
	region *hall_horiz = room_from_file("rooms/hall_horiz");
	add_standard_room(HALL_HORIZ, hall_horiz);
	region *hall_vert = room_from_file("rooms/hall_vert");
	add_standard_room(HALL_VERT, hall_vert);
	int x, y;
	char sprintf_fodder[256];
	for (x = 0; x < WORLD_DIM; ++x) {
		for (y = 0; y < WORLD_DIM; ++y) {
			sprintf(sprintf_fodder, "%d, %d", x, y);
			w.regions[x][y] = generate_region(sprintf_fodder, 0, 0);
			if (x % 2 == 0) {
				if (y % 2 == 0) {
					populate_region(w.regions[x][y], EMPTY);
				} else {
					populate_region(w.regions[x][y], HALL_VERT);
				}
			} else {
				if (y % 2 == 0) {
					populate_region(w.regions[x][y], HALL_HORIZ);
				} else {
					populate_region(w.regions[x][y], EMPTY);
				}
			}
		}
	}
	save_world(&w);
}

int main(int argc, char *argv[])
{
	make_world();
	return 0;
}
