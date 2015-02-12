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
#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "texture.h"
#include "entity.h"
#include "item.h"
#include "level.h"
#include "lightsource.h"
#include "sight.h"
#include "player.h"
#include "projectile.h"
#include "gui.h"
#include "fx.h"
#include "dungeon.h"
#include "scheduler.h"
#include "save.h"

static SDL_Window *SCREEN;
static SDL_GLContext *CONTEXT;
static bool RUNNING = false;
static const GLfloat CAMERA_SCALE = 1.0;
static int SCREEN_WIDTH = 0;
static int SCREEN_HEIGHT = 0;
static hash_map *MODE_CALLBACKS = NULL;
static mode *CURRENT_MODE = NULL;
static int LAST_TIME = 0;
static int CURRENT_TIME = 0;
static char CURRENT_DIALOG[256] = "";
static char *ENTERED_TEXT = NULL;
static Mix_Chunk *BLIP_SOUND;

SCM __api_reset_game()
{
	reset_game();
	return SCM_BOOL_F;
}

SCM __api_get_screen_width()
{
	return scm_from_int(get_screen_width());
}

SCM __api_get_screen_height()
{
	return scm_from_int(get_screen_height());
}

SCM __api_set_current_dialog(SCM d)
{
	char *t = scm_to_locale_string(d);
	set_current_dialog(t);
	free(t);
	return SCM_BOOL_F;
}

SCM __api_get_current_dialog()
{
	return scm_from_locale_string(get_current_dialog());
}

SCM __api_define_mode(SCM name, SCM init, SCM update, SCM draw)
{
	char *t = scm_to_locale_string(name);
	define_mode(t,
			make_thunk_scm(init),
			make_thunk_scm(update),
			make_thunk_scm(draw));
	free(t);
	return SCM_BOOL_F;
}

SCM __api_define_standard_gui_mode(SCM name, SCM init)
{
	char *t = scm_to_locale_string(name);
	define_mode(t,
			make_thunk_scm(init),
			make_thunk(mode_standard_gui_update),
			make_thunk(mode_standard_gui_draw));
	free(t);
	return SCM_BOOL_F;
}

SCM __api_set_running(SCM b)
{
	set_running(scm_to_bool(b));
	return SCM_BOOL_F;
}

SCM __api_set_mode(SCM mode)
{
	char *s = scm_to_locale_string(mode);
	set_mode(s);
	free(s);
	return SCM_BOOL_F;
}

void mode_standard_gui_update()
{
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
					mouse_clicked(event.button.x, event.button.y);
				}
				break;
			case SDL_QUIT:
				set_running(false);
				break;
		}
	}
}

void mode_standard_gui_draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	draw_gui();

	SDL_GL_SwapWindow(SCREEN);
}

static void mode_text_entry_update()
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
				set_running(false);
				break;
			case SDL_KEYDOWN:
				pressed = 1; //Intentional lack of break
				if (event.key.keysym.sym == SDLK_BACKSPACE && cursor != buffer) {
					*(--cursor) = 0x0;
					return;
				} else if (event.key.keysym.sym == SDLK_LEFT && cursor != buffer) {
					--cursor;
				} else if (event.key.keysym.sym == SDLK_RIGHT && cursor < buffer + 255 * sizeof(char)) {
					++cursor;
				} else if (event.key.keysym.sym == SDLK_RETURN) {
					set_mode("main");
					SDL_StopTextInput();
					return;
				}
				break;
			case SDL_KEYUP:
				if (!event.key.repeat) {
					pressed = pressed ? pressed : 0;
					if (event.key.keysym.sym == SDLK_F2) {
						take_screenshot("screenshot.png");
					} else if (event.key.keysym.sym == SDLK_w) {
						set_player_movement(pressed, DIR_NORTH);
					} else if (event.key.keysym.sym == SDLK_s) {
						set_player_movement(pressed, DIR_SOUTH);
					} else if (event.key.keysym.sym == SDLK_a) {
						set_player_movement(pressed, DIR_WEST);
					} else if (event.key.keysym.sym == SDLK_d) {
						set_player_movement(pressed, DIR_EAST);
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
}

static void mode_text_entry_draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glTranslatef(SCREEN_WIDTH/2 - (get_player_x() + get_player_stat("width")/2) * CAMERA_SCALE, SCREEN_HEIGHT/2 - (get_player_y() + get_player_stat("height")/2) * CAMERA_SCALE, 0);
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

	draw_gui();

	if (ENTERED_TEXT != NULL) {
		render_text_bitmap(ENTERED_TEXT, max((SCREEN_WIDTH - (4 * 8 * strlen(ENTERED_TEXT))) / 2, 0), SCREEN_HEIGHT - 4 * 8, 4);
	}

	SDL_GL_SwapWindow(SCREEN);
}

static SCM repl_catch_body(void *body_data)
{
	scm_c_eval_string((char *) ENTERED_TEXT);
	return SCM_BOOL_F;
}

static SCM repl_catch_handle(void *handler_data, SCM key, SCM parameters)
{
	log_err("Invalid command: %s", (char *) handler_data);
	return SCM_BOOL_F;
}

static void mode_main_update()
{
	if (ENTERED_TEXT != NULL) {
		scm_c_catch(SCM_BOOL_T,
				repl_catch_body, ENTERED_TEXT,
				repl_catch_handle, ENTERED_TEXT,
				NULL, NULL);
		free(ENTERED_TEXT);
		ENTERED_TEXT = NULL;
	}

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
						set_player_movement(pressed, DIR_NORTH);
					} else if (event.key.keysym.sym == SDLK_s) {
						set_player_movement(pressed, DIR_SOUTH);
					} else if (event.key.keysym.sym == SDLK_a) {
						set_player_movement(pressed, DIR_WEST);
					} else if (event.key.keysym.sym == SDLK_d) {
						set_player_movement(pressed, DIR_EAST);
					} else if (event.key.keysym.sym == SDLK_SPACE) {
						if (pressed) {
							set_current_dialog("");
							Mix_PlayChannel(-1, BLIP_SOUND, 0);
						}
					} else if (event.key.keysym.sym == SDLK_o) {
						set_mode("text_entry");
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
				use_player_item(1, calculate_angle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, event.button.x, event.button.y));
				mouse_clicked(event.button.x, event.button.y);
				spawn_global_fx(make_global_fx(global_effect_shake, 5));
				break;
			case SDL_MOUSEBUTTONUP:
				use_player_item(0, 0.0);
				break;
			case SDL_QUIT:
				set_running(false);
				break;
		}
	}
	update_player();
	update_entity();
	update_item();
	update_projectile();
	update_fx();
	update_sight();
}

static void mode_main_draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	//glTranslatef(SCREEN_WIDTH/2 - get_player_x(), SCREEN_HEIGHT/2 - get_player_y(), 0);
	glTranslatef(SCREEN_WIDTH/2 - (get_player_x() + get_player_stat("width")/2) * CAMERA_SCALE, SCREEN_HEIGHT/2 - (get_player_y() + get_player_stat("height")/2) * CAMERA_SCALE, 0);
	glScalef(CAMERA_SCALE, CAMERA_SCALE, 1.0);

	apply_global_fx();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

/*		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);*/
	/*glPushMatrix();
	glTranslatef(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 100, 0);
	glScalef(200, 200, 1);
	glColor4f(0, 0, 0, get_current_level()->ambience);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, get_standard_vertices_handler());
	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);*/
	/*glBegin(GL_TRIANGLES);
	glVertex2d(0, 0);
	glVertex2d(200, 200);
	glVertex2d(0, 200);
	glEnd();

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
		 glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE ); 
		 glStencilFunc( GL_EQUAL, 1, 1 );
		 glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );*/

	draw_level();
	draw_entity();
	draw_projectile();
	draw_player();
	draw_level_top();
	draw_fx();
	draw_lightsource();
	draw_sight();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

		glDisable( GL_STENCIL_TEST );

	draw_gui();


	SDL_GL_SwapWindow(SCREEN);
}

void initialize_game()
{
	MODE_CALLBACKS = make_hash_map();

	scm_c_define_gsubr("reset-game", 0, 0, 0, __api_reset_game);
	scm_c_define_gsubr("get-screen-width", 0, 0, 0, __api_get_screen_width);
	scm_c_define_gsubr("get-screen-height", 0, 0, 0, __api_get_screen_height);
	scm_c_define_gsubr("set-current-dialog", 1, 0, 0, __api_set_current_dialog);
	scm_c_define_gsubr("get-current-dialog", 0, 0, 0, __api_get_current_dialog);
	scm_c_define_gsubr("define-mode", 4, 0, 0, __api_define_mode);
	scm_c_define_gsubr("define-standard-gui-mode", 2, 0, 0, __api_define_standard_gui_mode);
	scm_c_define_gsubr("set-running", 1, 0, 0, __api_set_running);
	scm_c_define_gsubr("set-mode", 1, 0, 0, __api_set_mode);

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
	Mix_OpenAudio(22050, AUDIO_S16, 2, 4096);

	BLIP_SOUND = Mix_LoadWAV("sfx/blip.wav");

	initialize_utils();
	initialize_scheduler();
	initialize_dungeon();
	initialize_save();
	initialize_texture();
	initialize_item();
	initialize_lightsource();
	initialize_entity();
	initialize_player();
	initialize_projectile();
	initialize_fx();
	initialize_gui();
	initialize_level();
	initialize_sight();

	define_mode("text_entry",
			make_thunk(NULL),
			make_thunk(mode_text_entry_update),
			make_thunk(mode_text_entry_draw));
	define_mode("main",
			make_thunk_scm(scm_c_primitive_load("script/gui/main.scm")),
			make_thunk(mode_main_update),
			make_thunk(mode_main_draw));

	load_all("script/init");

	set_mode("main_menu");
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

	glClearStencil(0);
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

void define_mode(char *name, thunk init, thunk update, thunk draw)
{
	mode *m = (mode *) malloc(sizeof(mode));
	m->init = init;
	m->update = update;
	m->draw = draw;
	set_hash(MODE_CALLBACKS, name, m);
}

void set_current_dialog(char *text)
{
	strcpy(CURRENT_DIALOG, text);
}

char *get_current_dialog()
{
	return CURRENT_DIALOG;
}


void take_screenshot(char *path)
{
	SDL_Surface *image = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	int index;
	void *temp_row;
	 
	temp_row = malloc(image->pitch);
	for (index = 0; index < image->h/2; index++) {
		memcpy((Uint8 *) temp_row, (Uint8 *) image->pixels + image->pitch * index, image->pitch);
		memcpy((Uint8 *) image->pixels + image->pitch * index, (Uint8 *) image->pixels + image->pitch * (image->h - index-1), image->pitch);
		memcpy((Uint8 *) image->pixels + image->pitch * (image->h - index-1), temp_row, image->pitch);
	}
	free(temp_row);
	IMG_SavePNG(image, path);
	SDL_FreeSurface(image);
}

void set_running(bool b)
{
	RUNNING = b;
}

void set_mode(char *s)
{
	if ((CURRENT_MODE = get_hash(MODE_CALLBACKS, s)) != NULL) {
		execute_thunk(CURRENT_MODE->init);
	}
}

void reset_game()
{
	reset_gui();
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
	RUNNING = true;
	while (RUNNING) {
		mode m = *CURRENT_MODE;
		SDL_Delay(5);
		CURRENT_TIME = SDL_GetTicks();
		if (CURRENT_TIME - LAST_TIME > 40) {
			update_scheduler();
			execute_thunk(m.update);
			LAST_TIME = CURRENT_TIME;
		}
		execute_thunk(m.draw);
	}
	SDL_Quit();
}
