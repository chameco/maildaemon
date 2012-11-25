#ifndef CHARACTER_H
#define CHARACTER_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
#include "projectile.h"
#include "level.h"
void initialize_player();
void draw_player();
int get_player_x();
int get_player_y();
int get_player_w();
int get_player_h();
double get_player_health();
double get_player_max_health();
double get_player_magic();
double get_player_max_magic();
double get_player_exp();
double get_player_exp_to_next();
int get_player_level();
void hit_player(int dmg);
void give_player_exp(double exp);
void warp_player(int x, int y);
void shoot_left_player_weapon(int pressed);
void shoot_right_player_weapon(int pressed);
void shoot_both_player_weapons(int pressed);
void move_player_north(int pressed);
void move_player_south(int pressed);
void move_player_west(int pressed);
void move_player_east(int pressed);
mode update_player();
#endif
