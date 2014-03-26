#include "game.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "resources.h"
#include "repl.h"
#include "entity.h"
#include "weapon.h"
#include "level.h"
#include "lights.h"
#include "player.h"
#include "projectile.h"
#include "gui.h"
#include "fx.h"

SDL_Window *SCREEN;
SDL_GLContext *CONTEXT;
int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;
int GAME_OVER = 0;
mode CURRENT_MODE = MAIN_MENU_MODE;
int LAST_TIME = 0;
int CURRENT_TIME = 0;
char *CURRENT_DIALOG = NULL;
int DIALOG_TICKS = 0;
Mix_Chunk *BLIP_SOUND;

void initialize_game()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		log_err("Failed to initialize SDL");    
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if ((SCREEN = SDL_CreateWindow("maildaemon", 
					SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED,
					0, 0,
					SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN))
			== NULL ) {
		log_err("Failed to initialize window");    
		exit(1);
	}
	CONTEXT = SDL_GL_CreateContext(SCREEN);

	SDL_DisplayMode dmode;
	if (SDL_GetWindowDisplayMode(SCREEN, &dmode) != 0) {
		log_err("Unable to fetch display mode");
		exit(1);
	}
	SCREEN_WIDTH = dmode.w;
	SCREEN_HEIGHT = dmode.h;
	

	debug("SCREEN_WIDTH=%d, SCREEN_HEIGHT=%d", SCREEN_WIDTH, SCREEN_HEIGHT);

	initGL();
	IMG_Init(IMG_INIT_PNG);

	Mix_OpenAudio(22050, AUDIO_S16, 2, 4096);
	BLIP_SOUND = Mix_LoadWAV("sfx/blip.wav");

	initialize_utils();
	initialize_resources();
	initialize_repl();
	initialize_level();
	initialize_weapons();
	initialize_lights();
	initialize_entity();
	initialize_player();
	initialize_projectile();
	initialize_fx();
	initialize_gui();

	//scm_c_eval_string("(spawn-lightsource (make-lightsource 100 100 128 2 (make-color 0 1 0 1)))");
	scm_c_eval_string("(spawn-entity (make-entity 0 128 128 32 32 10 0 40))");

	set_current_dialog("Hello");
}

void initGL()
{
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		log_err("Initializing GLEW failed with the following error: %s", glewGetErrorString(glewError));
		exit(1);
	}

	if (!GLEW_VERSION_2_1) {
		log_err("OpenGL 2.1 not supported");
		exit(1);
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0, 0, 0, 0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

SDL_Window *get_screen()
{
	return SCREEN;
}

int get_screen_width()
{
	return SCREEN_WIDTH;
}

int get_screen_height()
{
	return SCREEN_HEIGHT;
}

char *get_current_dialog()
{
	return CURRENT_DIALOG;
}

void set_current_dialog(char *text)
{
	CURRENT_DIALOG = text;
}


void take_screenshot(char *path)
{
	SDL_Surface *image = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	int index;
	void *temp_row;
	 
	temp_row = (void *) malloc(image->pitch);
	if (temp_row == NULL) {
			log_err("Not enough memory for image inversion");
	}
	for(index = 0; index < image->h/2; index++)    {
		memcpy((Uint8 *) temp_row, (Uint8 *) image->pixels + image->pitch * index, image->pitch);
		memcpy((Uint8 *) image->pixels + image->pitch * index, (Uint8 *) image->pixels + image->pitch * (image->h - index-1), image->pitch);
		memcpy((Uint8 *) image->pixels + image->pitch * (image->h - index-1), temp_row, image->pitch);
	}
	free(temp_row);
	IMG_SavePNG(image, path);
	SDL_FreeSurface(image);
}

void set_mode(mode m)
{
	CURRENT_MODE = m;
}

void reset_game()
{
	reset_lights();
	reset_entities();
	reset_player();
	reset_projectile();
	reset_fx();
}

void main_game_loop()
{
	int running = 1;
	while (running) {
		SDL_Delay(10);
		switch (CURRENT_MODE) {
			case TITLE_MODE:
				running = draw_title_loop();
				break;
			case MAIN_MENU_MODE:
				running = draw_main_menu_loop();
				break;
			case DRAW_MODE:
				running = draw_main_loop();
				break;
			case GAME_OVER_MODE:
				running = draw_game_over_loop();
				break;
		}
	}
}

int draw_title_loop()
{
	static int ticks = 0;
	static resource *title_image;
	if (!title_image) {
		title_image = load_resource("textures/title.png");
	}
	CURRENT_TIME = SDL_GetTicks();
	if (CURRENT_TIME - LAST_TIME > 50) {
		if (ticks <= 100) {
			glClear(GL_COLOR_BUFFER_BIT);

			draw_resource(title_image, SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 200);

			SDL_GL_SwapWindow(SCREEN);
			ticks++;
		} else {
			CURRENT_MODE = MAIN_MENU_MODE;
		}
	}
	return 1;
}

int draw_main_menu_loop()
{
	static int loaded = 0;
	static resource *ampersand;
	static SDL_Rect button_enter_rect;
	static SDL_Rect button_quit_rect;
	if (!loaded) {
		loaded = 1;
		Mix_PlayChannel(-1, Mix_LoadWAV("sfx/mail.wav"), 0);
		ampersand = load_resource("textures/ampersand.png");
		button_enter_rect.w = 200;
		button_enter_rect.h = 50;
		button_enter_rect.x = SCREEN_WIDTH/2 - button_enter_rect.w/2;
		button_enter_rect.y = SCREEN_HEIGHT/2 - button_enter_rect.h/2;
		button_quit_rect.w = 200;
		button_quit_rect.h = 50;
		button_quit_rect.x = button_enter_rect.x;
		button_quit_rect.y = button_enter_rect.y + 75;

	}
	SDL_Rect mousecursor;
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_F2) {
					take_screenshot("screenshot.png");
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					mousecursor.w = mousecursor.h = 1;
					mousecursor.x = event.button.x;
					mousecursor.y = event.button.y;
					if (!check_collision(mousecursor, button_enter_rect)) {
						CURRENT_MODE = DRAW_MODE;
					}
					if (!check_collision(mousecursor, button_quit_rect)) {
						return 0;
					}
				}
				break;
			case SDL_QUIT:
				return 0;
				break;
		}
	}
	glClear(GL_COLOR_BUFFER_BIT);

	render_text_bitmap(SCREEN_WIDTH/2 - 640, 100, "maildaemon", 16.0);

	draw_resource(ampersand, button_enter_rect.x - 500, button_enter_rect.y);
	draw_resource(ampersand, button_enter_rect.x + button_enter_rect.w + 180, button_enter_rect.y);

	draw_button("Enter Game", button_enter_rect.x, button_enter_rect.y);
	draw_button("Quit", button_quit_rect.x, button_quit_rect.y);

	SDL_GL_SwapWindow(SCREEN);
	
	return 1;
}

void global_effect_shake()
{
	glTranslatef(rand() % 10, rand() % 10, 0);
}

int draw_main_loop()
{
	//static int counted_frames = 0;
	//static int start_ticks = 0;
	//if (start_ticks == 0) start_ticks = SDL_GetTicks();
	//debug("fps: %f", counted_frames / ((SDL_GetTicks() - start_ticks) / 1000.0f));
	//counted_frames++;

	int pressed;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		pressed = 0;
		switch (event.type) {
			case SDL_KEYDOWN:
				pressed = 1; //Intentional lack of break
			case SDL_KEYUP:
				if (!event.key.repeat) {
					pressed = pressed ? pressed : 0;
					if (event.key.keysym.sym == SDLK_F2) {
						take_screenshot("screenshot.png");
					} else if (event.key.keysym.sym == SDLK_w) {
						move_player_north(pressed);
					} else if (event.key.keysym.sym == SDLK_s) {
						move_player_south(pressed);
					} else if (event.key.keysym.sym == SDLK_a) {
						move_player_west(pressed);
					} else if (event.key.keysym.sym == SDLK_d) {
						move_player_east(pressed);
					} else if (event.key.keysym.sym == SDLK_UP) {
						shoot_player_weapon(pressed, NORTH);
					} else if (event.key.keysym.sym == SDLK_DOWN) {
						shoot_player_weapon(pressed, SOUTH);
					} else if (event.key.keysym.sym == SDLK_LEFT) {
						shoot_player_weapon(pressed, WEST);
					} else if (event.key.keysym.sym == SDLK_RIGHT) {
						shoot_player_weapon(pressed, EAST);
					} else if (event.key.keysym.sym == SDLK_SPACE) {
						if (pressed && get_current_dialog() != NULL) {
							set_current_dialog(NULL);
							Mix_PlayChannel(-1, BLIP_SOUND, 0);
						}
					} else if (event.key.keysym.sym == SDLK_o) {
						spawn_global_fx(make_global_fx(global_effect_shake, 10));
					} else if (event.key.keysym.sym == SDLK_0) {
						set_player_weapon_index(0);
					} else if (event.key.keysym.sym == SDLK_1) {
						set_player_weapon_index(1);
					} else if (event.key.keysym.sym == SDLK_2) {
						set_player_weapon_index(2);
					} else if (event.key.keysym.sym == SDLK_3) {
						set_player_weapon_index(3);
					} else if (event.key.keysym.sym == SDLK_4) {
						set_player_weapon_index(4);
					} else if (event.key.keysym.sym == SDLK_5) {
						set_player_weapon_index(5);
					} else if (event.key.keysym.sym == SDLK_6) {
						set_player_weapon_index(6);
					} else if (event.key.keysym.sym == SDLK_7) {
						set_player_weapon_index(7);
					} else if (event.key.keysym.sym == SDLK_8) {
						set_player_weapon_index(8);
					} else if (event.key.keysym.sym == SDLK_9) {
						set_player_weapon_index(9);
					}
				}
				break;
			case SDL_QUIT:
				return 0;
				break;
		}
	}
	CURRENT_TIME = SDL_GetTicks();
	if (CURRENT_TIME - LAST_TIME > 40) {
		update_repl();
		update_player();
		update_entity();
		update_weapons();
		update_projectile();
		update_fx();
		update_gui();
		LAST_TIME = CURRENT_TIME;
	}
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glTranslatef(SCREEN_WIDTH/2 - get_player_x(), SCREEN_HEIGHT/2 - get_player_y(), 0);

	apply_global_fx();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	draw_level();
	draw_entity();
	draw_player();
	draw_projectile();
	draw_fx();
	draw_lights();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	draw_gui();

	if (get_current_dialog() != NULL) {
		draw_dialog_box(get_current_dialog(), (SCREEN_WIDTH - 1000) / 2, SCREEN_HEIGHT - 100);
	}

	SDL_GL_SwapWindow(SCREEN);
	return 1;
}

int draw_game_over_loop()
{
	static int loaded = 0;
	static resource *game_over_text;
	static SDL_Rect button_respawn_rect;
	static SDL_Rect button_quit_rect;
	if (!loaded) {
		loaded = 1;
		game_over_text = load_resource("textures/gameover.png");
		button_respawn_rect.w = 200;
		button_respawn_rect.h = 50;
		button_respawn_rect.x = SCREEN_WIDTH/2 - 250;
		button_respawn_rect.y = SCREEN_HEIGHT/2 + 100;
		button_quit_rect.w = 200;
		button_quit_rect.h = 50;
		button_quit_rect.x = SCREEN_WIDTH/2 + 50;
		button_quit_rect.y = SCREEN_HEIGHT/2 + 100;

	}
	SDL_Rect mousecursor;
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					mousecursor.w = mousecursor.h = 1;
					mousecursor.x = event.button.x;
					mousecursor.y = event.button.y;
					if (!check_collision(mousecursor, button_respawn_rect)) {
						reset_game();
						CURRENT_MODE = MAIN_MENU_MODE;
					}
					else if (!check_collision(mousecursor, button_quit_rect)) {
						return 0;
					}
				}
				break;
			case SDL_QUIT:
				return 0;
				break;
			default:
				break;
		}
	}
	glClear(GL_COLOR_BUFFER_BIT);

	draw_resource(game_over_text, SCREEN_WIDTH/2 - 350, SCREEN_HEIGHT/2 - 100);

	draw_button("Main Menu", button_respawn_rect.x, button_respawn_rect.y);
	draw_button("Quit", button_quit_rect.x, button_quit_rect.y);

	SDL_GL_SwapWindow(SCREEN);
	return 1;
}
