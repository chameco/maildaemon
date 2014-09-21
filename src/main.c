#include <cuttle/debug.h>
#include <solid.h>

#include "dungeon.h"
#include "level.h"
#include "game.h"
#include "save.h"

void inner_main()
{
	initialize_game();
	main_game_loop();
}

int main(int argc, char *argv[])
{
	scm_boot_guile(argc, argv, inner_main, NULL);
}
