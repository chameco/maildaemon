#include <cuttle/debug.h>
#include <solid/solid.h>

#include "dungeon.h"
#include "level.h"
#include "game.h"
#include "save.h"

int main(int argc, char *argv[])
{
	initialize_game();
	main_game_loop();
}
