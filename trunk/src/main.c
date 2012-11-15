#include "game.h"
#include "level.h"

int main(int argc, char *argv[])
{
	initialize_game();
	load_level(0);
	main_game_loop();
	SDL_Quit();
	return 0;
}
