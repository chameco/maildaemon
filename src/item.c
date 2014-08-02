#include "item.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#include <SDL2/SDL_mixer.h>
#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "projectile.h"

static list_node *ITEMS;

static hash_map *ITEM_PROTOTYPES;

static scm_t_bits __api_item_tag;

SCM __api_build_item_prototype(SCM name, SCM update_func)
{
	char *n = scm_to_locale_string(name);
	item *w = build_item_prototype(n, update_func);
	free(n);
	return scm_new_smob(__api_item_tag, (unsigned long) w);
}

SCM __api_make_item(SCM name)
{
	char *n = scm_to_locale_string(name);
	item *w = make_item(n);
	free(n);
	SCM ret = scm_new_smob(__api_item_tag, (unsigned long) w);
	scm_gc_protect_object(ret);
	return ret;
}

SCM __api_is_item_active(SCM i)
{
	item *it = (item *) SCM_SMOB_DATA(i);
	return scm_from_bool(is_item_active(it));
}

SCM __api_get_item_charge(SCM i)
{
	item *it = (item *) SCM_SMOB_DATA(i);
	return scm_from_double(get_item_charge(it));
}

SCM __api_set_item_charge(SCM i, SCM c)
{
	item *it = (item *) SCM_SMOB_DATA(i);
	set_item_charge(it, scm_to_double(c));
	return SCM_BOOL_F;
}

SCM __api_get_item_rotation(SCM i)
{
	item *it = (item *) SCM_SMOB_DATA(i);
	return scm_from_double(get_item_rotation(it));
}

SCM __api_get_item_x(SCM i)
{
	item *it = (item *) SCM_SMOB_DATA(i);
	return scm_from_double(*(it->x));
}

SCM __api_get_item_y(SCM i)
{
	item *it = (item *) SCM_SMOB_DATA(i);
	return scm_from_double(*(it->y));
}

SCM __api_activate_item(SCM i, SCM rot)
{
	item *it = (item *) SCM_SMOB_DATA(i);
	activate_item(it, scm_to_double(rot));
	return SCM_BOOL_F;
}

SCM __api_deactivate_item(SCM i)
{
	item *it = (item *) SCM_SMOB_DATA(i);
	deactivate_item(it);
	return SCM_BOOL_F;
}

SCM __api_smob_item_mark(SCM i)
{
	item *it = (item *) SCM_SMOB_DATA(i);
	scm_gc_mark(it->draw_hud_func);
	scm_gc_mark(it->update_func);
	return it->data;
}

void initialize_item()
{
	ITEMS = make_list();
	ITEM_PROTOTYPES = make_hash_map();

	__api_item_tag = scm_make_smob_type("item", sizeof(item));
	scm_set_smob_mark(__api_item_tag, __api_smob_item_mark);
	scm_c_define_gsubr("build-item-prototype", 2, 0, 0, __api_build_item_prototype);
	scm_c_define_gsubr("make-item", 1, 0, 0, __api_make_item);
	scm_c_define_gsubr("is-item-active", 1, 0, 0, __api_is_item_active);
	scm_c_define_gsubr("get-item-charge", 1, 0, 0, __api_get_item_charge);
	scm_c_define_gsubr("set-item-charge", 2, 0, 0, __api_set_item_charge);
	scm_c_define_gsubr("get-item-rotation", 1, 0, 0, __api_get_item_rotation);
	scm_c_define_gsubr("get-item-x", 1, 0, 0, __api_get_item_x);
	scm_c_define_gsubr("get-item-y", 1, 0, 0, __api_get_item_y);
	scm_c_define_gsubr("activate-item", 2, 0, 0, __api_activate_item);
	scm_c_define_gsubr("deactivate-item", 1, 0, 0, __api_deactivate_item);

	DIR *d = opendir("script/items");
	struct dirent *entry;
	char buf[256];
	if (d != NULL) {
		while ((entry = readdir(d))) {
			char *pos = strrchr(entry->d_name, '.');
			if (pos != NULL && strcmp(pos + 1, "scm") == 0) {
				strcpy(buf, "script/items/");
				strcat(buf, entry->d_name);
				scm_c_primitive_load(buf);
			}
		}
		closedir(d);
	} else {
		log_err("Directory \"script/items\" does not exist");
		exit(1);
	}
}

scm_t_bits get_item_tag()
{
	return __api_item_tag;
}

void reset_item()
{
	list_node *c;
	for (c = ITEMS->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((item *) c->data) != NULL) {
			free_item((item *) c->data);
		}
	}
	ITEMS = make_list();
}

void free_item(item *i)
{
	free(i);
}

item *build_item_prototype(char *name, SCM update_func)
{
	item *ret = malloc(sizeof(item));
	ret->x = NULL;
	ret->y = NULL;
	ret->active = 0;
	ret->rotation = 0;
	//ret->sound = Mix_LoadWAV(sfx_path);
	ret->sound = NULL;
	ret->charge = 100.0;
	ret->update_func = update_func;
	ret->data = SCM_BOOL_F;
	set_hash(ITEM_PROTOTYPES, name, (void *) ret);
	return ret;
}

item *make_item(char *name)
{
	item *proto = (item *) get_hash(ITEM_PROTOTYPES, name);
	if (proto == NULL) {
		log_err("Item prototype \"%s\" does not exist", name);
		exit(1);
	}
	item *ret = malloc(sizeof(item));
	memcpy(ret, proto, sizeof(item));
	char buf[256] = "sfx/items/";
	strncat(buf, name, sizeof(buf) - strlen(buf) - 5);
	strcat(buf, ".wav");
	ret->sound = Mix_LoadWAV(buf);
	insert_list(ITEMS, (void *) ret);
	return ret;
}

bool is_item_active(item *i)
{
	return i->active;
}

double get_item_charge(item *i)
{
	return i->charge;
}

void set_item_charge(item *i, double c)
{
	i->charge = c;
}

double get_item_rotation(item *i)
{
	return i->rotation;
}

void activate_item(item *i, double rotation)
{
	if (i == NULL) {
		return;
	}
	i->rotation = rotation;
	i->active = 1;
	Mix_PlayChannel(-1, i->sound, 0);
}

void deactivate_item(item *i)
{
	if (i == NULL) {
		return;
	}
	i->active = 0;
}

void update_item()
{
	item *i;
	list_node *c;
	for (c = ITEMS; c->next != NULL; c = c->next) {
		if (((item *) c->data) != NULL) {
			i = (item *) c->data;
			i->data = scm_call_2(i->update_func, scm_new_smob(__api_item_tag, (unsigned long) i), i->data);
		}
	}
}
