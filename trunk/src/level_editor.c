#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

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
#include "game.h"

static int CURSOR_X = 0;
static int CURSOR_Y = 0;

SCM __api_swap_block_at_cursor(SCM block)
{
	get_current_level()->tiles[CURSOR_X][CURSOR_Y] = scm_to_int(block);
	return SCM_BOOL_F;
}

SCM __api_set_level_name(SCM name)
{
	char *s = scm_to_locale_string(name);
	strcpy(get_current_level()->name, s);
	free(s);
	return SCM_BOOL_F;
}

SCM __api_set_level_ambience(SCM a)
{
	get_current_level()->ambience = scm_to_double(a);
	return SCM_BOOL_F;
}

void inner_main(void *data, int argc, char *argv[])
{
	initialize_game();
	switch_level(argv[1]);
	scm_c_define_gsubr("swap-block", 1, 0, 0, __api_swap_block_at_cursor);
	scm_c_define_gsubr("set-level-name", 1, 0, 0, __api_set_level_name);
	scm_c_define_gsubr("set-level-ambience", 1, 0, 0, __api_set_level_ambience);
	int running = 1;
	resource *cursor = load_resource("textures/level_editor_cursor.png");
	while (running) {
		SDL_Delay(10);
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					if (!event.key.repeat) {
						if (event.key.keysym.sym == SDLK_F2) {
							take_screenshot("screenshot.png");
						} else if (event.key.keysym.sym == SDLK_w) {
							CURSOR_Y -= 1;
						} else if (event.key.keysym.sym == SDLK_a) {
							CURSOR_X -= 1;
						} else if (event.key.keysym.sym == SDLK_s) {
							CURSOR_Y += 1;
						} else if (event.key.keysym.sym == SDLK_d) {
							CURSOR_X += 1;
						} else if (event.key.keysym.sym == SDLK_x) {
							save_level(get_current_level());
						}
					}
					break;
				case SDL_QUIT:
					running = 0;
					break;
			}
		}

		update_repl();

		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glTranslatef(get_screen_width()/2 - CURSOR_X * TILE_DIM, get_screen_height()/2 - CURSOR_Y * TILE_DIM, 0);

		apply_global_fx();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		draw_level();
		draw_entity();
		draw_player();
		draw_projectile();
		draw_fx();
		draw_lights();

		draw_resource(cursor, CURSOR_X * TILE_DIM, CURSOR_Y * TILE_DIM);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		SDL_GL_SwapWindow(get_screen());
	}
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	if (argc <= 1) {
		log_err("Usage: level_editor <path>");
		exit(1);
	}
	scm_boot_guile(argc, argv, inner_main, NULL);
}
