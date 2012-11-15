#include "game.h"

SDL_Surface *screen;
int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;

void initialize_game()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_err("Failed to initialize SDL\n");    
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	const SDL_VideoInfo *info = SDL_GetVideoInfo();

	if ((screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
					info->vfmt->BitsPerPixel,
					SDL_OPENGL))
			== NULL ) {
		log_err("Failed to set video mode\n");    
	}
	SCREEN_WIDTH = screen->w;
	SCREEN_HEIGHT = screen->h;

	IMG_Init(IMG_INIT_PNG);
	initGL();

	SDL_WM_SetCaption("Purge", NULL);

	initialize_levels();
	initialize_enemies();
	initialize_player();
	initialize_projectiles();
}

void initGL()
{
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

void main_game_loop()
{
	int running = 1;
	int last_time = 0;
	int current_time = 0;
	int pressed;
	SDL_Event event;
	while (running) {
		while (SDL_PollEvent(&event)) {
			pressed = 0;
			switch (event.type) {
				case SDL_KEYDOWN:
					pressed = 1; //Intentional lack of break
				case SDL_KEYUP:
					pressed = pressed ? pressed : 0;
					if (event.key.keysym.sym == SDLK_w) {
						move_player_north(pressed);
					} else if (event.key.keysym.sym == SDLK_s) {
						move_player_south(pressed);
					} else if (event.key.keysym.sym == SDLK_a) {
						move_player_west(pressed);
					} else if (event.key.keysym.sym == SDLK_d) {
						move_player_east(pressed);
					} else if (event.key.keysym.sym == SDLK_SPACE) {
						shoot_player_weapon(pressed);
					}
					break;
				case SDL_QUIT:
					running = 0;
					break;
			}
		}
		current_time = SDL_GetTicks();
		if (current_time - last_time > 50) {
			update_player();
			update_enemies();
			update_projectiles();
			last_time = current_time;
		}
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glTranslatef(SCREEN_WIDTH/2 - get_player_x(), SCREEN_HEIGHT/2 - get_player_y(), 0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		draw_current_level();
		draw_enemies();
		draw_player();
		draw_projectiles();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		SDL_GL_SwapBuffers();
	}
}
