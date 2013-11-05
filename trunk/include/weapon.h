#ifndef PURGE_WEAPON_H
#define PURGE_WEAPON_H
#include <SDL2/SDL_mixer.h>
#include "utils.h"

typedef struct weapon {
	color c;
	int *x, *y;
	int xoffset, yoffset;
	int speed, damage;
	double charge;
	double max_charge;
	int isbeam;
	int bullets_per_volley;
	int pdim;
	int cooldown;
	int firing;
	int xv, yv;
	Mix_Chunk *sound;
	int channel;
} weapon;

void initialize_weapons();
weapon *make_weapon(color c, int *x, int *y, int xoffset, int yoffset,
		int speed, int damage, double max_charge, int isbeam, int bullets_per_volley,
		int pdim, char *sfx_path);
void press_trigger(weapon *w, direction d);
void release_trigger(weapon *w);
void update_weapons();
#endif
