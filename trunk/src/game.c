#include "game.h"

SDL_Surface *screen;
int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;
int GAME_OVER = 0;
mode CURRENT_MODE = TITLE_MODE;
int LAST_TIME = 0;
int CURRENT_TIME = 0;

void initialize_game()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_err("Failed to initialize SDL");    
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
		log_err("Failed to set video mode");    
	}
	SCREEN_WIDTH = screen->w;
	SCREEN_HEIGHT = screen->h;

	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	initGL();

	SDL_WM_SetCaption("Purge", NULL);

	initialize_utils();
	debug("INITIALIZED UTILS");
	initialize_enemy();
	debug("INITIALIZED ENEMY");
	initialize_ai();
	debug("INITIALIZED AI");
	initialize_player();
	debug("INITIALIZED PLAYER");
	initialize_projectile();
	debug("INITIALIZED PROJECTILE");
	initialize_entity();
	debug("INITIALIZED ENTITY");
	initialize_levels();
	debug("INITIALIZED LEVELS");
	initialize_gui();
	debug("INITIALIZED GUI");
	initialize_fx();
	debug("INITIALIZED EFFECTS");
}

void initGL()
{
	//Initialize GLEW
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		exit(1);
	}

	//Make sure OpenGL 2.1 is supported
	if (!GLEW_VERSION_2_1) {
		printf("OpenGL 2.1 not supported!\n");
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

void main_game_loop()
{
	int running = 1;
	while (running) {
		switch (CURRENT_MODE) {
			case TITLE_MODE:
				draw_title_loop();
				break;
			case MAIN_MENU_MODE:
				running = draw_main_menu_loop();
				break;
			case DRAW_MODE:
				running = draw_main_loop();
				break;
			case GAME_OVER_MODE:
				debug("GAME OVER");
				running = 0;
				break;
		}
	}
}

void draw_title_loop()
{
	static int ticks = 0;
	static GLuint title_image = 0;
	if (!title_image) {
		title_image = load_texture("textures/title.png");
	}
	CURRENT_TIME = SDL_GetTicks();
	if (CURRENT_TIME - LAST_TIME > 50) {
		if (ticks <= 100) {
			glClear(GL_COLOR_BUFFER_BIT);

			glPushMatrix();
			glTranslatef(SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 200, 0);
			glBindTexture(GL_TEXTURE_2D, title_image);

			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
			glTexCoord2i(1, 0); glVertex3f(400, 0, 0);
			glTexCoord2i(1, 1); glVertex3f(400, 400, 0);
			glTexCoord2i(0, 1); glVertex3f(0, 400, 0);
			glEnd();
			glPopMatrix();

			SDL_GL_SwapBuffers();
			ticks++;
		} else {
			CURRENT_MODE = MAIN_MENU_MODE;
		}
	}

}

int draw_main_menu_loop()
{
	static GLuint menu_background = 0;
	if (!menu_background) {
		menu_background = load_texture("textures/menu.png");
	}
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					CURRENT_MODE = DRAW_MODE;
				}
				break;
			case SDL_QUIT:
				return 0;
				break;
		}
	}
	glClear(GL_COLOR_BUFFER_BIT);

	//Draw Background
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glBindTexture(GL_TEXTURE_2D, menu_background);

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
	glTexCoord2i(1, 0); glVertex3f(SCREEN_WIDTH, 0, 0);
	glTexCoord2i(1, 1); glVertex3f(SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	glTexCoord2i(0, 1); glVertex3f(0, SCREEN_HEIGHT, 0);
	glEnd();
	glPopMatrix();

	//Draw Buttons
	glPushMatrix();
	glTranslatef(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 100, 0);

	SDL_GL_SwapBuffers();
	return 1;
}

int draw_main_loop()
{
	int pressed;
	SDL_Event event;
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
				} else if (event.key.keysym.sym == SDLK_UP) {
					shoot_player_weapon(pressed, NORTH);
				} else if (event.key.keysym.sym == SDLK_DOWN) {
					shoot_player_weapon(pressed, SOUTH);
				} else if (event.key.keysym.sym == SDLK_LEFT) {
					shoot_player_weapon(pressed, WEST);
				} else if (event.key.keysym.sym == SDLK_RIGHT) {
					shoot_player_weapon(pressed, EAST);
				} else if (event.key.keysym.sym == SDLK_SPACE) {
					melee_player_weapon(pressed, get_player_facing());
				}
				break;
			case SDL_QUIT:
				return 0;
				break;
		}
	}
	CURRENT_TIME = SDL_GetTicks();
	if (CURRENT_TIME - LAST_TIME > 50) {
		CURRENT_MODE = update_player();
		update_ai();
		update_enemy();
		update_projectile();
		update_entity();
		update_fx();
		update_gui();
		LAST_TIME = CURRENT_TIME;
	}
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glTranslatef(SCREEN_WIDTH/2 - get_player_x(), SCREEN_HEIGHT/2 - get_player_y(), 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	draw_current_level();
	draw_enemy();
	draw_player();
	draw_projectile();
	draw_fx();
	draw_entity();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	draw_gui();

	SDL_GL_SwapBuffers();
	return 1;
}
