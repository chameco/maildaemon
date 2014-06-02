#pragma once

#include <stdbool.h>

#include "utils.h"
#include "item.h"
#include "game.h"

void initialize_player();
void reset_player();
int get_player_x();
int get_player_y();
int get_player_w();
int get_player_h();
int get_player_facing();
double get_player_health();
double get_player_max_health();
double get_player_charge_percent();
double get_player_exp();
double get_player_exp_to_next();
void set_player_item(int i, item *it);
item *get_player_item();
int get_player_level();
void hit_player(int dmg);
void give_player_exp(double exp);
void warp_player(int x, int y);
void set_player_item_index(int i);
void use_player_item(bool pressed, double xv, double yv);
void set_player_movement(bool pressed, direction d);
void update_player();
void draw_player();
