#pragma once

#include <SDL2/SDL.h>

typedef enum mode {
	TITLE_MODE,
	MAIN_MENU_MODE,
	TEXT_ENTRY_MODE,
	DRAW_MODE,
	GAME_OVER_MODE
} mode;

void initialize_game();
void initGL();
SDL_Window *get_screen();
int get_screen_width();
int get_screen_height();
char *get_current_dialog();
void set_current_dialog(char *text);
void take_screenshot(char *path);
void set_mode(mode m);
void reset_game();
void main_game_loop();
int draw_title_loop();
int draw_main_menu_loop();
int draw_text_entry_loop();
int draw_main_loop();
int draw_game_over_loop();
