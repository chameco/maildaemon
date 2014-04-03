#include "weapon.h"

#include <SDL2/SDL_mixer.h>
#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "projectile.h"

list_node *WEAPONS;

scm_t_bits __api_weapon_tag;

SCM __api_make_weapon(SCM c, SCM xoffset, SCM yoffset,
		SCM speed, SCM damage, SCM max_charge, SCM isbeam, SCM bullets_per_volley,
		SCM pdim, SCM sfx_path)
{
	color *col = (color *) SCM_SMOB_DATA(c);
	char *sfxp = scm_to_locale_string(sfx_path);
	weapon *w = make_weapon(*col, scm_to_int(xoffset), scm_to_int(yoffset),
			scm_to_int(speed), scm_to_int(damage), scm_to_double(max_charge),
			scm_to_int(isbeam), scm_to_int(bullets_per_volley), scm_to_int(pdim), sfxp);
	free(sfxp);
	return scm_new_smob(__api_weapon_tag, (unsigned long) w);
}

SCM __api_press_trigger(SCM w, SCM d)
{
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	press_trigger(weap, scm_to_int(d));
	return SCM_BOOL_F;
}

SCM __api_release_trigger(SCM w)
{
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	release_trigger(weap);
	return SCM_BOOL_F;
}

void initialize_weapons()
{
	WEAPONS = make_list();

	__api_weapon_tag = scm_make_smob_type("weapon", sizeof(weapon));
	scm_c_define_gsubr("make-weapon", 10, 0, 0, __api_make_weapon);
	scm_c_define_gsubr("press-trigger", 2, 0, 0, __api_press_trigger);
	scm_c_define_gsubr("release-trigger", 1, 0, 0, __api_release_trigger);
}

weapon *make_weapon(color c, int xoffset, int yoffset,
		int speed, int damage, double max_charge, int isbeam, int bullets_per_volley,
		int pdim, char *sfx_path)
{
	weapon *ret = scm_gc_malloc(sizeof(weapon), "weapon");
	insert_list(WEAPONS, (void *) ret);
	ret->c = c;
	ret->x = NULL;
	ret->y = NULL;
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
