#pragma once

#include <SDL2/SDL_mixer.h>
#include <libguile.h>
#include "utils.h"

typedef struct weapon {
	int *x, *y;
	int firing;
	double xv, yv;
	Mix_Chunk *sound;
	double charge;
	SCM draw_hud_func;
	SCM update_func;
	SCM data;
	/*int xoffset, yoffset;
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
	int channel;*/
} weapon;

void initialize_weapon();
scm_t_bits get_weapon_tag();
weapon *make_weapon(char *sfx_path, SCM update_func);
int is_weapon_firing(weapon *w);
double get_weapon_charge(weapon *w);
void set_weapon_charge(weapon *w, double c);
double get_weapon_xv(weapon *w);
double get_weapon_yv(weapon *w);
void press_trigger(weapon *w, double xv, double yv);
void release_trigger(weapon *w);
void update_weapons();
