#pragma once

#include <stdbool.h>

#include "utils.h"
#include "texture.h"
#include "item.h"
#include "game.h"

void initialize_player();
void reset_player();

double get_player_x();
double get_player_y();

double get_player_stat(char *k);
void set_player_stat(char *k, double v);

item *get_player_item();
void set_player_item(int i, item *it);

void hit_player(int dmg);
void give_player_exp(double exp);
void warp_player(double x, double y);
void set_player_item_index(int i);
void use_player_item(bool pressed, double rot);
void set_player_movement(bool pressed, direction d);
void update_player();
void draw_player();
