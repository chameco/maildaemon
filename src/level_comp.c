#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "dungeon.h"
#include "level.h"

int main(int argc, char *argv[])
{
	char *name = argv[1];
	char *file = argv[2];
	debug("%s %s", name, file);
	FILE *f = fopen(file, "r");

	set_dungeon_noload("temple");

	level *l = malloc(sizeof(level));
	memset(l->tiles, 0, sizeof(l->tiles));
	strncpy(l->name, name, 31);
	l->ambience = 0.5;

	int x = 0;
	int y = 0;
	char ch = 0;

	while (ch != EOF)
	{
		ch = fgetc(f);
		if (ch == '\n') {
			y += 1;
			x = 0;
		} else {
			switch (ch) {
				case ' ':
					l->tiles[x][y] = VOID;
					break;
				case '.':
					l->tiles[x][y] = STONEFLOOR;
					break;
				case '%':
					l->tiles[x][y] = STONEFLOOR;
					break;
				case '=':
					l->tiles[x][y] = PLANKS;
					break;
				case ',':
					l->tiles[x][y] = GRASS;
					break;
				case '-':
					l->tiles[x][y] = SNOW;
					break;
				case '~':
					l->tiles[x][y] = SAND;
					break;
				case '_':
					l->tiles[x][y] = CARPET;
					break;
				case '#':
					l->tiles[x][y] = STONE;
					break;
				case '|':
					l->tiles[x][y] = STONETOP;
					break;
				case '!':
					l->tiles[x][y] = STONETORCH;
					break;
				case '@':
					l->tiles[x][y] = STATUE;
					break;
				case 'o':
					l->tiles[x][y] = SHRUB;
					break;
				case 'w':
					l->tiles[x][y] = WATER;
					break;
				default:
					break;
			}
			x += 1;
		}
	}

	save_level(l);

	return 0;
}
