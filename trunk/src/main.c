#include <SDL2/SDL.h>
#include <cuttle/debug.h>
#include <libguile.h>
#include "game.h"

void inner_main()
{
	initialize_game();
	main_game_loop();
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	scm_boot_guile(argc, argv, inner_main, NULL);
}
