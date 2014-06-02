#pragma once

#include <stdbool.h>

#include <SDL2/SDL_mixer.h>
#include <libguile.h>

#include "utils.h"

typedef struct item {
	char name[256];
	int *x, *y;
	bool active;
	double xv, yv;
	Mix_Chunk *sound;
	double charge;
	SCM draw_hud_func;
	SCM update_func;
	SCM data;
} item;

void initialize_item();
void reset_item();
void free_item(item *i);
scm_t_bits get_item_tag();
item *build_item_prototype(char *name, SCM update_func);
item *make_item(char *name);
int is_item_active(item *i);
double get_item_charge(item *i);
void set_item_charge(item *i, double c);
double get_item_xv(item *i);
double get_item_yv(item *i);
void activate_item(item *i, double xv, double yv);
void deactivate_item(item *i);
void update_item();
