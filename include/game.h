#pragma once

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <solid/solid.h>

#include "utils.h"

typedef struct mode {
	thunk init;
	thunk update;
	thunk draw;
} mode;

void initialize_game();
void initGL();
SDL_Window *get_screen();
int get_screen_width();
int get_screen_height();
void mode_standard_gui_update();
void mode_standard_gui_draw();
void define_mode(char *name, thunk init, thunk update, thunk draw);
void set_current_dialog(char *text);
char *get_current_dialog();
void take_screenshot(char *path);
void set_running(bool b);
void set_mode(char *s);
void reset_game();
void main_game_loop();
