#include "inventory.h"

int CURRENT_SLOT;
item *INVENTORY[9] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void add_item_slot(int slot, item *i)
{
	INVENTORY[slot] = i;
}

int is_slot_full(int slot)
{
	return (INVENTORY[slot] == NULL);
}

item *get_item_slot(int slot)
{
	return INVENTORY[slot];
}

void use_current_item()
{
}
