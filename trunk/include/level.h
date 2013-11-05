#ifndef PURGE_LEVEL_H
#define PURGE_LEVEL_H
#include "utils.h"
#include "worldgen.h"

world *get_world();

int get_block_dim();
void initialize_level();
void change_current_region(int rx, int ry, direction d);
void update_level();
void draw_level();
#endif
