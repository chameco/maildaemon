#ifndef GUI_H
#define GUI_H
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
typedef struct effect {
	float r, g, b, a;
	int radius;
	float duration;
} effect;
void initialize_effects();
void spawn_effect(float r, float g, float b, float a,
		int radius, float duration);
#endif
