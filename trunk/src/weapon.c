#include "weapon.h"

#include <SDL2/SDL_mixer.h>
#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "projectile.h"

static list_node *WEAPONS;

static scm_t_bits __api_weapon_tag;

SCM __api_make_weapon(SCM sfx_path, SCM update_func)
{
	char *sfxp = scm_to_locale_string(sfx_path);
	weapon *w = make_weapon(sfxp, update_func);
	free(sfxp);
	SCM ret = scm_new_smob(__api_weapon_tag, (unsigned long) w);
	scm_gc_protect_object(ret);
	return ret;
}

SCM __api_is_weapon_firing(SCM w)
{
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	if (is_weapon_firing(weap)) {
		return SCM_BOOL_T;
	} else {
		return SCM_BOOL_F;
	}
}

SCM __api_get_weapon_charge(SCM w)
{
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	return scm_from_double(get_weapon_charge(weap));
}

SCM __api_set_weapon_charge(SCM w, SCM c)
{
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	set_weapon_charge(weap, scm_to_double(c));
	return SCM_BOOL_F;
}

SCM __api_get_weapon_xv(SCM w)
{
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	return scm_from_int(get_weapon_xv(weap));
}

SCM __api_get_weapon_yv(SCM w)
{
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	return scm_from_int(get_weapon_yv(weap));
}

SCM __api_get_weapon_x(SCM w)
{
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	return scm_from_int(*(weap->x));
}

SCM __api_get_weapon_y(SCM w)
{
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	return scm_from_int(*(weap->y));
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

SCM __api_smob_weapon_mark(SCM w)
{
	weapon *wep = (weapon *) SCM_SMOB_DATA(w);
	return wep->update_func;
}

void initialize_weapons()
{
	WEAPONS = make_list();

	__api_weapon_tag = scm_make_smob_type("weapon", sizeof(weapon));
	scm_set_smob_mark(__api_weapon_tag, __api_smob_weapon_mark);
	scm_c_define_gsubr("make-weapon", 2, 0, 0, __api_make_weapon);
	scm_c_define_gsubr("is-weapon-firing", 1, 0, 0, __api_is_weapon_firing);
	scm_c_define_gsubr("get-weapon-charge", 1, 0, 0, __api_get_weapon_charge);
	scm_c_define_gsubr("set-weapon-charge", 2, 0, 0, __api_set_weapon_charge);
	scm_c_define_gsubr("get-weapon-xv", 1, 0, 0, __api_get_weapon_xv);
	scm_c_define_gsubr("get-weapon-yv", 1, 0, 0, __api_get_weapon_yv);
	scm_c_define_gsubr("get-weapon-x", 1, 0, 0, __api_get_weapon_x);
	scm_c_define_gsubr("get-weapon-y", 1, 0, 0, __api_get_weapon_y);
	scm_c_define_gsubr("press-trigger", 2, 0, 0, __api_press_trigger);
	scm_c_define_gsubr("release-trigger", 1, 0, 0, __api_release_trigger);
}

scm_t_bits get_weapon_tag()
{
	return __api_weapon_tag;
}

weapon *make_weapon(char *sfx_path, SCM update_func)
{
	weapon *ret = scm_gc_malloc(sizeof(weapon), "weapon");
	insert_list(WEAPONS, (void *) ret);
	ret->x = NULL;
	ret->y = NULL;
	ret->firing = 0;
	ret->xv = ret->yv = 0;
	ret->sound = Mix_LoadWAV(sfx_path);
	ret->charge = 100.0;
	ret->update_func = update_func;
	ret->data = SCM_BOOL_F;
	return ret;
}

int is_weapon_firing(weapon *w)
{
	return w->firing;
}

double get_weapon_charge(weapon *w)
{
	return w->charge;
}

void set_weapon_charge(weapon *w, double c)
{
	w->charge = c;
}

int get_weapon_xv(weapon *w)
{
	return w->xv;
}

int get_weapon_yv(weapon *w)
{
	return w->yv;
}

void press_trigger(weapon *w, direction d)
{
	if (w == NULL) {
		return;
	}
	switch (d) {
		case NORTH:
			w->xv = 0;
			w->yv = -1;
			break;
		case SOUTH:
			w->xv = 0;
			w->yv = 1;
			break;
		case WEST:
			w->xv = -1;
			w->yv = 0;
			break;
		case EAST:
			w->xv = 1;
			w->yv = 0;
			break;
	}
	w->firing = 1;
	Mix_PlayChannel(-1, w->sound, 0);
}

void release_trigger(weapon *w)
{
	if (w == NULL) {
		return;
	}
	w->firing = 0;
}

void update_weapons()
{
	weapon *w;
	list_node *c;
	for (c = WEAPONS->next; c->next != NULL; c = c->next) {
		if (((weapon *) c->data) != NULL) {
			w = (weapon *) c->data;
			w->data = scm_call_2(w->update_func, scm_new_smob(__api_weapon_tag, (unsigned long) w), w->data);
		}
	}
}
