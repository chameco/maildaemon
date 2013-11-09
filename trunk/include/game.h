#ifndef PURGE_GAME_H
#define PURGE_GAME_H

typedef enum mode {
	TITLE_MODE,
	MAIN_MENU_MODE,
	DRAW_MODE,
	GAME_OVER_MODE
} mode;

void initialize_game();
void initGL();
void take_screenshot(char *path);
void reset_game();
void main_game_loop();
void draw_title_loop();
int draw_main_menu_loop();
int draw_main_loop();
int draw_game_over_loop();
#endif
