#include "game.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "texture.h"
#include "entity.h"
#include "item.h"
#include "level.h"
#include "lightsource.h"
#include "player.h"
#include "projectile.h"
#include "gui.h"
#include "fx.h"

static SDL_Window *SCREEN;
static SDL_GLContext *CONTEXT;
static const GLfloat CAMERA_SCALE = 1.0;
static int SCREEN_WIDTH = 0;
static int SCREEN_HEIGHT = 0;
static mode CURRENT_MODE = MAIN_MENU_MODE;
static int LAST_TIME = 0;
static int CURRENT_TIME = 0;
static char *CURRENT_DIALOG = NULL;
static char *ENTERED_TEXT = NULL;
static Mix_Chunk *BLIP_SOUND;
static SCM HUD = SCM_BOOL_F;

void __api_load_module(SCM path)
{
	char *t = scm_to_locale_string(path);
	scm_c_primitive_load(t);
	free(t);
}

void initialize_game()
{
	scm_c_define_gsubr("load-module", 1, 0, 0, __api_load_module);

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
	
	initGL();
	IMG_Init(IMG_INIT_PNG);
	Mix_Init(MIX_INIT_OGG);
	SDLNet_Init();

	Mix_OpenAudio(22050, AUDIO_S16, 2, 4096);
	BLIP_SOUND = Mix_LoadWAV("sfx/blip.wav");

	initialize_utils();
	initialize_texture();
	initialize_level();
	initialize_item();
	initialize_lightsource();
	initialize_entity();
	initialize_player();
	initialize_projectile();
	initialize_fx();
	initialize_gui();

	HUD = scm_c_primitive_load("script/gui/hud.scm");

	set_current_dialog("Hello");


	//spawn_fx(make_fx(SMOKE_CONST, COLOR_GRAY,
	//			80, 0, 8, 50, 100));

	Mix_PlayMusic(Mix_LoadMUS("music/menu.ogg"), -1);
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
	 
	temp_row = malloc(image->pitch);
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
	reset_lightsource();
	reset_item();
	reset_entity();
	reset_projectile();
	reset_fx();
	reset_player();
	reset_level();
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
			case TEXT_ENTRY_MODE:
				running = draw_text_entry_loop();
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
	static texture *title_image;
	if (!title_image) {
		title_image = load_texture("textures/title.png", 0, 0);
	}
	CURRENT_TIME = SDL_GetTicks();
	if (CURRENT_TIME - LAST_TIME > 50) {
		if (ticks <= 100) {
			glClear(GL_COLOR_BUFFER_BIT);

			draw_texture(title_image, SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 200);

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
	static texture *ampersand;
	static SDL_Rect button_enter_rect;
	static SDL_Rect button_quit_rect;
	if (!loaded) {
		loaded = 1;
		Mix_PlayChannel(-1, Mix_LoadWAV("sfx/mail.wav"), 0);
		ampersand = load_texture("textures/ampersand.png", 0, 0);
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

	render_text_bitmap("maildaemon", SCREEN_WIDTH/2 - 640, 100, 16.0);

	draw_texture(ampersand, button_enter_rect.x - 500, button_enter_rect.y);
	draw_texture(ampersand, button_enter_rect.x + button_enter_rect.w + 180, button_enter_rect.y);

	draw_button("Enter Game", button_enter_rect.x, button_enter_rect.y);
	draw_button("Quit", button_quit_rect.x, button_quit_rect.y);

	SDL_GL_SwapWindow(SCREEN);
	
	return 1;
}

int draw_text_entry_loop()
{
	static char *buffer, *cursor;
	int len;
	if (ENTERED_TEXT == NULL) {
		SDL_StartTextInput();
		buffer = (char *) malloc(sizeof(char) * 256);
		cursor = buffer;
		memset(buffer, 0, sizeof(char) * 256);
		ENTERED_TEXT = cursor;
	}
	SDL_Event event;
	bool pressed;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				return false;
				break;
			case SDL_KEYDOWN:
				pressed = 1; //Intentional lack of break
				if (event.key.keysym.sym == SDLK_BACKSPACE && cursor != buffer) {
					*(--cursor) = 0x0;
					return true;
				} else if (event.key.keysym.sym == SDLK_LEFT && cursor != buffer) {
					--cursor;
				} else if (event.key.keysym.sym == SDLK_RIGHT && cursor < buffer + 255 * sizeof(char)) {
					++cursor;
				} else if (event.key.keysym.sym == SDLK_RETURN) {
					set_mode(DRAW_MODE);
					SDL_StopTextInput();
					return true;
				}
				break;
			case SDL_KEYUP:
				if (!event.key.repeat) {
					pressed = pressed ? pressed : 0;
					if (event.key.keysym.sym == SDLK_F2) {
						take_screenshot("screenshot.png");
					} else if (event.key.keysym.sym == SDLK_w) {
						set_player_movement(pressed, NORTH);
					} else if (event.key.keysym.sym == SDLK_s) {
						set_player_movement(pressed, SOUTH);
					} else if (event.key.keysym.sym == SDLK_a) {
						set_player_movement(pressed, WEST);
					} else if (event.key.keysym.sym == SDLK_d) {
						set_player_movement(pressed, EAST);
					}
				}
				break;
			case SDL_TEXTINPUT:
				len = strlen(event.text.text) * sizeof(char);
				if (cursor + len < buffer + 255 * sizeof(char)) {
					memcpy(cursor, event.text.text, len);
					cursor += len;
				}
				break;
		}
	}
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glTranslatef(SCREEN_WIDTH/2 - (get_player_x() + get_player_w()/2) * CAMERA_SCALE, SCREEN_HEIGHT/2 - (get_player_y() + get_player_h()/2) * CAMERA_SCALE, 0);
	glScalef(CAMERA_SCALE, CAMERA_SCALE, 1.0);

	apply_global_fx();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	draw_level();
	draw_entity();
	draw_projectile();
	draw_player();
	draw_level_top();
	draw_fx();
	draw_lightsource();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	scm_call_0(HUD);

	render_text_bitmap(buffer, (SCREEN_WIDTH - (4 * 8 * strlen(buffer))) / 2, 0, 4);

	if (get_current_dialog() != NULL) {
		draw_dialog_box(get_current_dialog(), (SCREEN_WIDTH - 1000) / 2, SCREEN_HEIGHT - 100);
	}

	SDL_GL_SwapWindow(SCREEN);
	return 1;
}

int draw_main_loop()
{
	if (ENTERED_TEXT != NULL) {
		scm_c_eval_string(ENTERED_TEXT);
		free(ENTERED_TEXT);
		ENTERED_TEXT = NULL;
	}

	int pressed;
	double theta;
	int centerx, centery;
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
						set_player_movement(pressed, NORTH);
					} else if (event.key.keysym.sym == SDLK_s) {
						set_player_movement(pressed, SOUTH);
					} else if (event.key.keysym.sym == SDLK_a) {
						set_player_movement(pressed, WEST);
					} else if (event.key.keysym.sym == SDLK_d) {
						set_player_movement(pressed, EAST);
					} else if (event.key.keysym.sym == SDLK_SPACE) {
						if (pressed && get_current_dialog() != NULL) {
							set_current_dialog(NULL);
							Mix_PlayChannel(-1, BLIP_SOUND, 0);
						}
					} else if (event.key.keysym.sym == SDLK_o) {
						scm_c_eval_string("(get-player-x)");
						set_mode(TEXT_ENTRY_MODE);
					} else if (event.key.keysym.sym == SDLK_m) {
						hit_player(1000);
					} else if (event.key.keysym.sym == SDLK_0) {
						set_player_item_index(0);
					} else if (event.key.keysym.sym == SDLK_1) {
						set_player_item_index(1);
					} else if (event.key.keysym.sym == SDLK_2) {
						set_player_item_index(2);
					} else if (event.key.keysym.sym == SDLK_3) {
						set_player_item_index(3);
					} else if (event.key.keysym.sym == SDLK_4) {
						set_player_item_index(4);
					} else if (event.key.keysym.sym == SDLK_5) {
						set_player_item_index(5);
					} else if (event.key.keysym.sym == SDLK_6) {
						set_player_item_index(6);
					} else if (event.key.keysym.sym == SDLK_7) {
						set_player_item_index(7);
					} else if (event.key.keysym.sym == SDLK_8) {
						set_player_item_index(8);
					} else if (event.key.keysym.sym == SDLK_9) {
						set_player_item_index(9);
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				centerx = SCREEN_WIDTH/2;
				centery = SCREEN_HEIGHT/2;
				if (event.button.x >= centerx) {
					theta = atan((double) (event.button.y - centery)
							/ (double) (event.button.x - centerx));
				} else {
					theta = atan((double) (event.button.y - centery)
							/ (double) (event.button.x - centerx))
						+ 3.141592654;
				}
				use_player_item(1, theta);
				spawn_global_fx(make_global_fx(global_effect_shake, 5));
				break;
			case SDL_MOUSEBUTTONUP:
				use_player_item(0, 0.0);
				break;
			case SDL_QUIT:
				return 0;
				break;
		}
	}
	CURRENT_TIME = SDL_GetTicks();
	if (CURRENT_TIME - LAST_TIME > 40) {
		update_level();
		update_player();
		update_entity();
		update_item();
		update_projectile();
		update_fx();
		LAST_TIME = CURRENT_TIME;
	}
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	//glTranslatef(SCREEN_WIDTH/2 - get_player_x(), SCREEN_HEIGHT/2 - get_player_y(), 0);
	glTranslatef(SCREEN_WIDTH/2 - (get_player_x() + get_player_w()/2) * CAMERA_SCALE, SCREEN_HEIGHT/2 - (get_player_y() + get_player_h()/2) * CAMERA_SCALE, 0);
	glScalef(CAMERA_SCALE, CAMERA_SCALE, 1.0);

	apply_global_fx();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	draw_level();
	draw_entity();
	draw_projectile();
	draw_player();
	draw_level_top();
	draw_fx();
	draw_lightsource();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	scm_call_0(HUD);

	if (get_current_dialog() != NULL) {
		draw_dialog_box(get_current_dialog(), (SCREEN_WIDTH - 1000) / 2, SCREEN_HEIGHT - 100);
	}

	SDL_GL_SwapWindow(SCREEN);
	return 1;
}

int draw_game_over_loop()
{
	static int loaded = 0;
	static SDL_Rect button_respawn_rect;
	static SDL_Rect button_quit_rect;
	if (!loaded) {
		loaded = 1;
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

	render_text_bitmap("Game Over", SCREEN_WIDTH/2 - 576, 100, 16.0);

	draw_button("Main Menu", button_respawn_rect.x, button_respawn_rect.y);
	draw_button("Quit", button_quit_rect.x, button_quit_rect.y);

	SDL_GL_SwapWindow(SCREEN);
	return 1;
}
