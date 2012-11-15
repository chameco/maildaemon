#ifndef ENEMY_H
#define ENEMY_H
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
typedef struct enemy {
	int x;
	int y;
	int w;
	int h;
	GLuint tex;
	int health;
} enemy;
void initialize_enemies();
void update_enemies();
void draw_enemies();
#endif
