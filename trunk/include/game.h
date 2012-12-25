#ifndef GAME_H
#define GAME_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
#include "level.h"
#include "player.h"
#include "enemy.h"
#include "ai.h"
#include "projectile.h"
#include "gui.h"
#include "fx.h"

void initialize_game();
void initGL();
void main_game_loop();
void draw_title_loop();
void draw_main_loop();
#endif
