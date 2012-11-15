#ifndef CHARACTER_H
#define CHARACTER_H
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
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
void hit_player(int dmg);
void warp_player(int x, int y);
void shoot_player_weapon(int pressed);
void move_player_north(int pressed);
void move_player_south(int pressed);
void move_player_west(int pressed);
void move_player_east(int pressed);
void update_player();
#endif
