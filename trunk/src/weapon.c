#include "weapon.h"

#include <SDL2/SDL_mixer.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>
#include <solid/solid.h>

#include "vm.h"
#include "utils.h"
#include "projectile.h"

list_node *WEAPONS;

void PAPI_make_weapon(solid_vm *vm)
{
	char *sfx_path = solid_get_str_value(solid_pop_stack(vm));
	int pdim = solid_get_int_value(solid_pop_stack(vm));
	int bullets_per_volley = solid_get_int_value(solid_pop_stack(vm));
	int isbeam = solid_get_int_value(solid_pop_stack(vm));
	int max_charge = solid_get_int_value(solid_pop_stack(vm));
	int damage = solid_get_int_value(solid_pop_stack(vm));
	int speed = solid_get_int_value(solid_pop_stack(vm));
	int yoffset = solid_get_int_value(solid_pop_stack(vm));
	int xoffset = solid_get_int_value(solid_pop_stack(vm));
	int *y = (int *) solid_get_struct_value(solid_pop_stack(vm));
	int *x = (int *) solid_get_struct_value(solid_pop_stack(vm));
	color c = *((color *) solid_get_struct_value(solid_pop_stack(vm)));
	vm->regs[255] = solid_struct(vm, make_weapon(c, x, y, xoffset, yoffset, speed, damage, max_charge, isbeam, bullets_per_volley, pdim, sfx_path));
}
PAPI_2param(press_trigger, solid_get_struct_value, solid_get_int_value)
PAPI_1param(release_trigger, solid_get_struct_value)

void initialize_weapons_api()
{
	solid_object *ns = make_namespace("Weapon");
	define_function(ns, "make_weapon", PAPI_make_weapon);
	define_function(ns, "press_trigger", PAPI_press_trigger);
	define_function(ns, "release_trigger", PAPI_release_trigger);
}

void initialize_weapons()
{
	WEAPONS = make_list();
	initialize_weapons_api();
}

weapon *make_weapon(color c, int *x, int *y, int xoffset, int yoffset,
		int speed, int damage, double max_charge, int isbeam, int bullets_per_volley,
		int pdim, char *sfx_path)
{
	weapon *ret = (weapon *) malloc(sizeof(weapon));
	insert_list(WEAPONS, (void *) ret);
	ret->c = c;
	ret->x = x;
	ret->y = y;
	ret->xoffset = xoffset;
	ret->yoffset = yoffset;
	ret->speed = speed;
	ret->damage = damage;
	ret->charge = ret->max_charge = max_charge;
	ret->isbeam = isbeam;
	ret->bullets_per_volley = bullets_per_volley;
	ret->pdim = pdim;
	ret->sound = Mix_LoadWAV(sfx_path);
	ret->channel = -1;
	ret->cooldown = 0;
	ret->firing = 0;
	return ret;
}

void press_trigger(weapon *w, direction d)
{
	if (w == NULL) {
		return;
	}
	if (w->cooldown == 0) {
		switch (d) {
			case NORTH:
				w->xv = 0;
				w->yv = -w->speed;
				break;
			case SOUTH:
				w->xv = 0;
				w->yv = w->speed;
				break;
			case WEST:
				w->xv = -w->speed;
				w->yv = 0;
				break;
			case EAST:
				w->xv = w->speed;
				w->yv = 0;
				break;
		}
		w->firing = 1;
		if (w->isbeam) {
			if (w->channel == -1) {
				w->channel = Mix_PlayChannel(-1, w->sound, -1);
			}
		} else {
			w->cooldown = 5;
		}
	}
}
void release_trigger(weapon *w)
{
	if (w == NULL) {
		return;
	}
	if (w->isbeam) {
		w->firing = 0;
		Mix_HaltChannel(w->channel);
		w->channel = -1;
	}
}
void update_weapons()
{
	weapon *w;
	list_node *c;
	for (c = WEAPONS->next; c->next != NULL; c = c->next) {
		if (((weapon *) c->data) != NULL) {
			w = (weapon *) c->data;
			if (w->isbeam == 1) {
				if (w->firing) {
					if (w->charge >= 0.5) {
						w->charge -= 0.5;
					} else {
						w->firing = 0;
						continue;
					}
					if (abs(w->xv) > abs(w->yv)) {
						spawn_projectile(w->c, *(w->x)+w->xoffset, *(w->y)+w->yoffset,
								w->xv, w->yv, w->pdim*2, w->pdim, 100, w, w->damage);
					} else {
						spawn_projectile(w->c, *(w->x)+w->xoffset, *(w->y)+w->yoffset,
								w->xv, w->yv, w->pdim, w->pdim*2, 100, w, w->damage);
					}
				} else {
					if (w->charge <= 99.9) {
						w->charge += 0.1;
					}
				}
			} else {
				if (w->cooldown > 0) {
					w->cooldown--;
				}
				if (w->firing) {
					if (w->charge >= 5.0) {
						w->charge -= 5.0;
						spawn_projectile(w->c, *(w->x)+w->xoffset, *(w->y)+w->yoffset,
								w->xv, w->yv, w->pdim, w->pdim, 100, w, w->damage);
						Mix_PlayChannel(-1, w->sound, 0);
						w->firing = 0;
					} else {
						w->firing = 0;
					}
				} else {
					if (w->charge <= 99.9) {
						w->charge += 0.1;
					}
				}
			}
		}
	}
}
