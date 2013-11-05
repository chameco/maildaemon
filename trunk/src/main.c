#include <SDL2/SDL.h>
#include <cuttle/debug.h>
#include "game.h"

int main(int argc, char *argv[])
{
	initialize_game();
	main_game_loop();
	SDL_Quit();
	return 0;
}
