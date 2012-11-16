#ifndef GUI_H
#define GUI_H
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
void initialize_gui();
void update_gui();
void draw_gui();
#endif
