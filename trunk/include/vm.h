#ifndef PURGE_VM_H
#define PURGE_VM_H
#include <solid/solid.h>

#define PAPI_var(rtype, v) \
void PAPI_ ## v (solid_vm *vm) \
{ \
	vm->regs[255] = rtype(vm, v); \
}

#define PAPI_ptr_var(rtype, v) \
void PAPI_ ## v (solid_vm *vm) \
{ \
	vm->regs[255] = rtype(vm, &v); \
}

#define PAPI_getter(rtype, f) \
void PAPI_ ## f (solid_vm *vm) \
{ \
	vm->regs[255] = rtype(vm, f()); \
}

#define PAPI_1param(f, get_param1) \
void PAPI_ ## f (solid_vm *vm) \
{ \
	solid_object *a = solid_pop_stack(vm); \
	f(get_param1(a)); \
	vm->regs[255] = solid_int(vm, 0); \
}

#define PAPI_1param_with_return(rtype, f, get_param1) \
void PAPI_ ## f (solid_vm *vm) \
{ \
	solid_object *a = solid_pop_stack(vm); \
	vm->regs[255] = rtype(vm, f(get_param1(a))); \
}

#define PAPI_2param(f, get_param1, get_param2) \
void PAPI_ ## f (solid_vm *vm) \
{ \
	solid_object *b = solid_pop_stack(vm); \
	solid_object *a = solid_pop_stack(vm); \
	f(get_param1(a), get_param2(b)); \
	vm->regs[255] = solid_int(vm, 0); \
}

#define PAPI_2param_with_return(rtype, f, get_param1, get_param2) \
void PAPI_ ## f (solid_vm *vm) \
{ \
	solid_object *b = solid_pop_stack(vm); \
	solid_object *a = solid_pop_stack(vm); \
	vm->regs[255] = rtype(vm, f(get_param1(a), get_param2(b))); \
}

#define PAPI_field_from_struct(rtype, s, fname) \
void PAPI_ ## s ## _ ## fname (solid_vm *vm) \
{ \
	solid_object *a = solid_pop_stack(vm); \
	vm->regs[255] = rtype(vm, ((s *) solid_get_struct_value(a))->fname); \
}

void preinit_load_module(char *path);
void initialize_vm();
solid_object *make_namespace(char *name);
void define_function(solid_object *ns, char *name, void(*f)(solid_vm *vm));
solid_vm *get_vm();
solid_object *load_module(char *code);
solid_object *run_code(char *code);
#endif
