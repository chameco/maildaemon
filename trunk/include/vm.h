#ifndef PURGE_VM_H
#define PURGE_VM_H
#include <libguile.h>

void initialize_vm();
SCM load_module(char *code);
SCM run_code(char *code);
#endif
