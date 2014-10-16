#pragma once

#include <stdbool.h>

#include <SDL2/SDL_mixer.h>
#include <solid/solid.h>

#include "utils.h"

typedef struct item {
	double *x, *y;
	bool active;
	double rotation;
	Mix_Chunk *sound;
	double charge;
	solid_object *update_func;
} item;

void initialize_item();
void reset_item();
void free_item(item *i);
item *build_item_prototype(char *name, solid_object *update_func);
item *make_item(char *name);
bool is_item_active(item *i);
double get_item_charge(item *i);
void set_item_charge(item *i, double c);
double get_item_rotation(item *i);
void activate_item(item *i, double rotation);
void deactivate_item(item *i);
void update_item();
