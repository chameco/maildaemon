#include "vm.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <cuttle/debug.h>
#include <cuttle/utils.h>
#include <solid/solid.h>

solid_vm *VM;
list_node *MODULES;

void preinit_load_module(char *path)
{
	insert_list(MODULES, (void *) path);
}

void initialize_vm()
{
	MODULES = make_list();
	VM = solid_make_vm();
	list_node *c;
	for (c = MODULES->next; c->next != NULL; c = c->next) {
		if ((char *) c->data != NULL) {
			load_module((char *) c->data);
		}
	}
}

solid_object *make_namespace(char *name)
{
	solid_object *ns = solid_clone_object(VM->namespace_stack[VM->namespace_stack_pointer]);
	solid_set_namespace(solid_get_current_namespace(VM), solid_str(name), ns);
	return ns;
}

solid_vm *get_vm()
{
	return VM;
}

solid_object *load_module(char *path)
{
	solid_object *func = solid_parse_tree(solid_parse_file(path));
	solid_call_func(VM, func);
	solid_delete_object(func);
	return VM->regs[255];
}

solid_object *run_code(char *code)
{
	solid_object *func = solid_parse_tree(solid_parse_expr(code));
	solid_call_func(VM, func);
	solid_delete_object(func);
	return VM->regs[255];
}
