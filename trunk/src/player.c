#include "player.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "vm.h"
#include "utils.h"
#include "resources.h"
#include "worldgen.h"
#include "entity.h"
#include "weapon.h"
#include "game.h"
#include "level.h"
#include "projectile.h"
#include "lights.h"

int PLAYER_X = 0;
int PLAYER_Y = 0;
int NORTH_PRESSED = 0;
int SOUTH_PRESSED = 0;
int WEST_PRESSED = 0;
int EAST_PRESSED = 0;
const int PLAYER_MOVE_SPEED = 6;//Pixels per second
const int DIAG_SPEED = 5;
const int PLAYER_WIDTH = 32;
const int PLAYER_HEIGHT = 32;
direction PLAYER_FACING = 0;
double PLAYER_EXP = 0;
double PLAYER_EXP_TO_NEXT = 100;
int PLAYER_LEVEL = 0;
double PLAYER_MAX_HEALTH = 100;
double PLAYER_HEALTH;
double PLAYER_REGEN = 0.1;
resource *PLAYER_RESOURCES[4];
resource *PLAYER_ALTERNATE[4];
const int PLAYER_ANIM_STEP_TIMING = 8;
int PLAYER_ANIM_STEP = 0;
color PLAYER_COLOR;
weapon *PLAYER_WEAPONS[10] = {NULL};
int PLAYER_WEAPON_INDEX = 0;

PAPI_getter(solid_int, get_player_x)
PAPI_getter(solid_int, get_player_y)
PAPI_getter(solid_int, get_player_w)
PAPI_getter(solid_int, get_player_h)
PAPI_getter(solid_int, get_player_facing)

PAPI_getter(solid_double, get_player_health)
PAPI_getter(solid_double, get_player_max_health)
PAPI_getter(solid_double, get_player_magic)
PAPI_getter(solid_double, get_player_max_magic)
PAPI_getter(solid_double, get_player_exp)
PAPI_getter(solid_double, get_player_exp_to_next)

PAPI_getter(solid_struct, get_player_weapon)

PAPI_getter(solid_int, get_player_level)

PAPI_1param(hit_player, solid_get_int_value)
PAPI_1param(give_player_exp, solid_get_double_value)
PAPI_2param(warp_player, solid_get_int_value, solid_get_int_value)
PAPI_1param(set_player_weapon_index, solid_get_int_value)
PAPI_2param(shoot_player_weapon, solid_get_int_value, solid_get_int_value)

PAPI_1param(move_player_north, solid_get_int_value)
PAPI_1param(move_player_south, solid_get_int_value)
PAPI_1param(move_player_west, solid_get_int_value)
PAPI_1param(move_player_east, solid_get_int_value)

void initialize_player_api()
{
	solid_object *ns = make_namespace("Player");
	define_function(ns, "get_player_x", PAPI_get_player_x);
	define_function(ns, "get_player_y", PAPI_get_player_y);
	define_function(ns, "get_player_w", PAPI_get_player_w);
	define_function(ns, "get_player_h", PAPI_get_player_h);
	define_function(ns, "get_player_facing", PAPI_get_player_facing);

	define_function(ns, "get_player_health", PAPI_get_player_health);
	define_function(ns, "get_player_max_health", PAPI_get_player_max_health);
	define_function(ns, "get_player_magic", PAPI_get_player_magic);
	define_function(ns, "get_player_max_magic", PAPI_get_player_max_magic);
	define_function(ns, "get_player_exp", PAPI_get_player_exp);
	define_function(ns, "get_player_exp_to_next", PAPI_get_player_exp_to_next);

	define_function(ns, "get_player_weapon", PAPI_get_player_weapon);

	define_function(ns, "get_player_level", PAPI_get_player_level);

	define_function(ns, "hit_player", PAPI_hit_player);
	define_function(ns, "give_player_exp", PAPI_give_player_exp);
	define_function(ns, "warp_player", PAPI_warp_player);
	define_function(ns, "set_player_weapon_index", PAPI_set_player_weapon_index);
	define_function(ns, "shoot_player_weapon", PAPI_shoot_player_weapon);

	define_function(ns, "move_player_north", PAPI_move_player_north);
	define_function(ns, "move_player_south", PAPI_move_player_south);
	define_function(ns, "move_player_west", PAPI_move_player_west);
	define_function(ns, "move_player_east", PAPI_move_player_east);
}

void initialize_player()
{
	PLAYER_HEALTH = PLAYER_MAX_HEALTH;
	PLAYER_COLOR = COLOR_RED;

	PLAYER_RESOURCES[NORTH] = load_resource("textures/player/default/n.png");
	PLAYER_RESOURCES[SOUTH] = load_resource("textures/player/default/s.png");
	PLAYER_RESOURCES[WEST] = load_resource("textures/player/default/w.png");
	PLAYER_RESOURCES[EAST] = load_resource("textures/player/default/e.png");

	PLAYER_ALTERNATE[NORTH] = load_resource("textures/player/default/na.png");
	PLAYER_ALTERNATE[SOUTH] = load_resource("textures/player/default/sa.png");
	PLAYER_ALTERNATE[WEST] = load_resource("textures/player/default/wa.png");
	PLAYER_ALTERNATE[EAST] = load_resource("textures/player/default/ea.png");
	PLAYER_WEAPONS[0] = make_weapon(PLAYER_COLOR, &PLAYER_X, &PLAYER_Y, 8, 8, 16, 8, 100.0, 0, 1, 8, "sfx/laser.wav");
	PLAYER_WEAPONS[1] = make_weapon(COLOR_WHITE, &PLAYER_X, &PLAYER_Y, 8, 8, 16, 2, 100.0, 1, 1, 8, "sfx/beam.wav");

	initialize_player_api();
}

void reset_player()
{
	
	NORTH_PRESSED = SOUTH_PRESSED = WEST_PRESSED = EAST_PRESSED = 0;
	PLAYER_X = PLAYER_Y = 0;
	PLAYER_FACING = 0;
	PLAYER_EXP = 0;
	PLAYER_EXP_TO_NEXT = 100;
	PLAYER_LEVEL = 0;
	PLAYER_HEALTH = PLAYER_MAX_HEALTH;
	PLAYER_ANIM_STEP = 0;
	PLAYER_WEAPON_INDEX = 0;
}

int get_player_x()
{
	return PLAYER_X;
}

int get_player_y()
{
	return PLAYER_Y;
}

int get_player_w()
{
	return PLAYER_WIDTH;
}

int get_player_h()
{
	return PLAYER_HEIGHT;
}

int get_player_facing()
{
	return PLAYER_FACING;
}

double get_player_health()
{
	return PLAYER_HEALTH;
}

double get_player_max_health()
{
	return PLAYER_MAX_HEALTH;
}

double get_player_magic()
{
	return PLAYER_WEAPONS[PLAYER_WEAPON_INDEX]->charge;
}

double get_player_max_magic()
{
	return PLAYER_WEAPONS[PLAYER_WEAPON_INDEX]->max_charge;
}
double get_player_exp()
{
	return PLAYER_EXP;
}

double get_player_exp_to_next()
{
	return PLAYER_EXP_TO_NEXT;
}

int get_player_level()
{
	return PLAYER_LEVEL;
}

weapon *get_player_weapon()
{
	return PLAYER_WEAPONS[PLAYER_WEAPON_INDEX];
}

void hit_player(int dmg)
{
	PLAYER_HEALTH -= dmg;
}

void give_player_exp(double exp)
{
	PLAYER_EXP += exp;
	if (PLAYER_EXP >= PLAYER_EXP_TO_NEXT) {
		PLAYER_EXP = PLAYER_EXP - PLAYER_EXP_TO_NEXT;
		PLAYER_EXP_TO_NEXT += 50;
		PLAYER_LEVEL += 1;
		PLAYER_HEALTH = PLAYER_MAX_HEALTH;
	}
}

void warp_player(int x, int y)
{
	PLAYER_X = x;
	PLAYER_Y = y;
	//NORTH_PRESSED = SOUTH_PRESSED = WEST_PRESSED = EAST_PRESSED = 0;
}

void set_player_weapon_index(int i)
{
	release_trigger(PLAYER_WEAPONS[PLAYER_WEAPON_INDEX]);
	if (PLAYER_WEAPONS[i] != NULL) {
		PLAYER_WEAPON_INDEX = i;
	}
}

void shoot_player_weapon(int pressed, direction d)
{
	if (pressed) {
		press_trigger(PLAYER_WEAPONS[PLAYER_WEAPON_INDEX], d);
	} else {
		release_trigger(PLAYER_WEAPONS[PLAYER_WEAPON_INDEX]);
	}
}

void move_player_north(int pressed)
{
	if (pressed) {
		NORTH_PRESSED = 1;
		PLAYER_FACING = NORTH;
	} else {
		NORTH_PRESSED = 0;
	}
}

void move_player_south(int pressed)
{
	if (pressed) {
		SOUTH_PRESSED = 1;
		PLAYER_FACING = SOUTH;
	} else {
		SOUTH_PRESSED = 0;
	}
}

void move_player_west(int pressed)
{
	if (pressed) {
		WEST_PRESSED = 1;
		PLAYER_FACING = WEST;
	} else {
		WEST_PRESSED = 0;
	}
}

void move_player_east(int pressed)
{
	if (pressed) {
		EAST_PRESSED = 1;
		PLAYER_FACING = EAST;
	} else {
		EAST_PRESSED = 0;
	}
}

mode update_player()
{
	if (PLAYER_HEALTH <= 0.0) {
		return GAME_OVER_MODE;
	}

	if (NORTH_PRESSED || SOUTH_PRESSED || WEST_PRESSED || EAST_PRESSED) {
		PLAYER_ANIM_STEP = ((PLAYER_ANIM_STEP + 1) % PLAYER_ANIM_STEP_TIMING);
	}

	if (PLAYER_HEALTH <= PLAYER_MAX_HEALTH-PLAYER_REGEN) {
		PLAYER_HEALTH += PLAYER_REGEN;
	} else if (PLAYER_HEALTH < PLAYER_MAX_HEALTH) {
		PLAYER_HEALTH = PLAYER_MAX_HEALTH;
	}

	int tempx = PLAYER_X;
	int tempy = PLAYER_Y;

	if (NORTH_PRESSED) {
		if (WEST_PRESSED || EAST_PRESSED) {
			tempy -= DIAG_SPEED;
		} else {
			tempy -= PLAYER_MOVE_SPEED;
		}
	}

	if (SOUTH_PRESSED) {
		if (WEST_PRESSED || EAST_PRESSED) {
			tempy += DIAG_SPEED;
		} else {
			tempy += PLAYER_MOVE_SPEED;
		}
	}

	if (WEST_PRESSED) {
		if (NORTH_PRESSED || SOUTH_PRESSED) {
			tempx -= DIAG_SPEED;
		} else {
			tempx -= PLAYER_MOVE_SPEED;
		}
	}

	if (EAST_PRESSED) {
		if (NORTH_PRESSED || SOUTH_PRESSED) {
			tempx += DIAG_SPEED;
		} else {
			tempx += PLAYER_MOVE_SPEED;
		}
	}

	int xmodulo = tempx % 8;
	int ymodulo = tempy % 8;

	if (xmodulo != 0) {
		if (xmodulo <= 4) {
			tempx -= xmodulo;
		} else {
			tempx += (8 - xmodulo);
		}
	}

	if (ymodulo != 0) {
		if (ymodulo <= 4) {
			tempy -= ymodulo;
		} else {
			tempy += (8 - ymodulo);
		}
	}

	SDL_Rect player = {
		PLAYER_X,
		PLAYER_Y,
		PLAYER_WIDTH,
		PLAYER_HEIGHT
	};
	SDL_Rect b;
	int shouldmovex = 1;
	int shouldmovey = 1;
	int curmovex = 0;
	int curmovey = 0;

	int blockdim = get_block_dim();
	int xmin = (PLAYER_X/blockdim)-2;
	int ymin = (PLAYER_Y/blockdim)-2;
	int xmax = (PLAYER_X/blockdim)+2;
	int ymax = (PLAYER_Y/blockdim)+2;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_block(get_current_region(get_world()), x, y)) {
				b.x = x*32;
				b.y = y*32;
				b.w = b.h = blockdim;
				player.x = tempx;
				player.y = PLAYER_Y;
				curmovex = check_collision(player, b);
				shouldmovex = shouldmovex ? curmovex : 0;
				player.x = PLAYER_X;
				player.y = tempy;
				curmovey = check_collision(player, b);
				shouldmovey = shouldmovey ? curmovey : 0;
				player.y = PLAYER_Y;
			}
		}
	}

	list_node *entities = get_entities();
	list_node *c;
	for (c = entities->next; c->next != NULL; c = c->next) {
		if (((entity *) c->data) != NULL) {
			b.x = ((entity *) c->data)->x;
			b.y = ((entity *) c->data)->y;
			b.w = ((entity *) c->data)->w;
			b.h = ((entity *) c->data)->h;
			player.x = tempx;
			curmovex = check_collision(player, b);
			shouldmovex = shouldmovex ? curmovex : 0;
			player.x = PLAYER_X;
			player.y = tempy;
			curmovey = check_collision(player, b);
			shouldmovey = shouldmovey ? curmovey : 0;
			player.y = PLAYER_Y;
			if (!curmovex || !curmovey) {
				collide_entity((entity *) c->data);
			}
		}
	}

	if (shouldmovex) {
		PLAYER_X = tempx;
	}
	if (shouldmovey) {
		PLAYER_Y = tempy;
	}
	return DRAW_MODE;
}

void draw_player()
{
	if (PLAYER_ANIM_STEP < PLAYER_ANIM_STEP_TIMING / 2) {
		draw_resource(PLAYER_RESOURCES[PLAYER_FACING], PLAYER_X, PLAYER_Y);
	} else {
		draw_resource(PLAYER_ALTERNATE[PLAYER_FACING], PLAYER_X, PLAYER_Y);
	}
}
