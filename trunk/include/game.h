#ifndef GAME_H
#define GAME_H
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "level.h"
#include "character.h"
#include "enemy.h"
#include "projectile.h"
#include "gui.h"

void initialize_game();
void initGL();
void main_game_loop();
#endif
