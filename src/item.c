#include "item.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#include <SDL2/SDL_mixer.h>
#include <solid/solid.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "projectile.h"

static list_node *ITEMS;

static hash_map *ITEM_PROTOTYPES;

void __api_build_item_prototype(solid_vm *vm)
{
	solid_object *update_func = solid_pop_stack(vm);
	char *name = solid_get_str_value(solid_pop_stack(vm));
	item *w = build_item_prototype(name, update_func);
	vm->regs[255] = solid_struct(vm, w);
}

void __api_make_item(solid_vm *vm)
{
	char *name = solid_get_str_value(solid_pop_stack(vm));
	item *w = make_item(name);
	solid_object *ret = solid_struct(vm, w);
	vm->regs[255] = ret;
}

void __api_is_item_active(solid_vm *vm)
{
	item *it = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_bool(vm, is_item_active(it));
}

void __api_get_item_charge(solid_vm *vm)
{
	item *it = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, get_item_charge(it));
}

void __api_set_item_charge(solid_vm *vm)
{
	double c = solid_get_double_value(solid_pop_stack(vm));
	item *it = solid_get_struct_value(solid_pop_stack(vm));
	set_item_charge(it, c);
}

void __api_get_item_rotation(solid_vm *vm)
{
	item *it = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, get_item_rotation(it));
}

void __api_get_item_x(solid_vm *vm)
{
	item *it = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, *(it->x));
}

void __api_get_item_y(solid_vm *vm)
{
	item *it = solid_get_struct_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, *(it->y));
}

void __api_activate_item(solid_vm *vm)
{
	double rot = solid_get_double_value(solid_pop_stack(vm));
	item *it = solid_get_struct_value(solid_pop_stack(vm));
	activate_item(it, rot);
}

void __api_deactivate_item(solid_vm *vm)
{
	item *it = solid_get_struct_value(solid_pop_stack(vm));
	deactivate_item(it);
}

void initialize_item()
{
	ITEMS = make_list();
	ITEM_PROTOTYPES = make_hash_map();

	defun("build_item_prototype", __api_build_item_prototype);
	defun("make_item", __api_make_item);
	defun("is_item_active", __api_is_item_active);
	defun("get_item_charge", __api_get_item_charge);
	defun("set_item_charge", __api_set_item_charge);
	defun("get_item_rotation", __api_get_item_rotation);
	defun("get_item_x", __api_get_item_x);
	defun("get_item_y", __api_get_item_y);
	defun("activate_item", __api_activate_item);
	defun("deactivate_item", __api_deactivate_item);

	load_all("script/items");
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

item *build_item_prototype(char *name, solid_object *update_func)
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
	solid_mark_object(ret->update_func, 2);
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
	solid_vm *vm = get_vm();
	for (c = ITEMS; c->next != NULL; c = c->next) {
		if (((item *) c->data) != NULL) {
			i = (item *) c->data;
			solid_push_stack(vm, solid_struct(vm, i));
			solid_call_func(vm, i->update_func);
		}
	}
}
