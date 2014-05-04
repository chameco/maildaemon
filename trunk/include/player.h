#ifndef PURGE_CHARACTER_H
#define PURGE_CHARACTER_H
#include "utils.h"
#include "weapon.h"
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
void set_player_weapon(int i, weapon *w);
weapon *get_player_weapon();
int get_player_level();
void hit_player(int dmg);
void give_player_exp(double exp);
void warp_player(int x, int y);
void set_player_weapon_index(int i);
void shoot_player_weapon(int pressed, direction d);
void move_player_north(int pressed);
void move_player_south(int pressed);
void move_player_west(int pressed);
void move_player_east(int pressed);
void update_player();
void draw_player();
#endif
