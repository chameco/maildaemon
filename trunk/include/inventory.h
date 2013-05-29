#ifndef INVENTORY_H
#define INVENTORY_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"

typedef struct item {
	int id;
} item;

void add_item_slot(int slot, item *i);
int is_slot_full(int slot);
item *get_item_slot(int slot);

#endif
