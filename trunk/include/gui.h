#ifndef GUI_H
#define GUI_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
#include "player.h"
#include "level.h"
void initialize_gui();
void update_gui();
void draw_button(int id, char *text, int x, int y);
void draw_gui();
#endif
