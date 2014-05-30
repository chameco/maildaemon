#include "player.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "texture.h"
#include "entity.h"
#include "weapon.h"
#include "game.h"
#include "level.h"

static int PLAYER_X = 0;
static int PLAYER_Y = 0;
static int NORTH_PRESSED = 0;
static int SOUTH_PRESSED = 0;
static int WEST_PRESSED = 0;
static int EAST_PRESSED = 0;
static const int PLAYER_MOVE_SPEED = 6; //Pixels per second
static const int DIAG_SPEED = 5;
static const int PLAYER_WIDTH = 32;
static const int PLAYER_HEIGHT = 32;
static direction PLAYER_FACING = 0;
static double PLAYER_EXP = 0;
static double PLAYER_EXP_TO_NEXT = 100;
static int PLAYER_LEVEL = 0;
static double PLAYER_MAX_HEALTH = 100;
static double PLAYER_HEALTH;
static double PLAYER_REGEN = 0.1;
static texture *PLAYER_RESOURCES[4];
static texture *PLAYER_ALTERNATE[4];
static const int PLAYER_ANIM_STEP_TIMING = 8;
static int PLAYER_ANIM_STEP = 0;
static weapon *PLAYER_WEAPONS[10] = {NULL};
static int PLAYER_WEAPON_INDEX = 0;

SCM __api_get_player_x()
{
	return scm_from_int(get_player_x());
}

SCM __api_get_player_y()
{
	return scm_from_int(get_player_y());
}

SCM __api_get_player_w()
{
	return scm_from_int(get_player_w());
}

SCM __api_get_player_h()
{
	return scm_from_int(get_player_h());
}

SCM __api_get_player_facing()
{
	return scm_from_int(get_player_facing());
}

SCM __api_get_player_health()
{
	return scm_from_double(get_player_health());
}

SCM __api_get_player_max_health()
{
	return scm_from_double(get_player_max_health());
}

SCM __api_get_player_charge_percent()
{
	return scm_from_double(get_player_charge_percent());
}

SCM __api_get_player_exp()
{
	return scm_from_double(get_player_exp());
}

SCM __api_get_player_exp_to_next()
{
	return scm_from_double(get_player_exp_to_next());
}

SCM __api_set_player_weapon(SCM i, SCM w)
{
	weapon *weap = (weapon *) SCM_SMOB_DATA(w);
	set_player_weapon(scm_to_int(i), weap);
	return SCM_BOOL_F;
}

SCM __api_get_player_weapon()
{
	return scm_new_smob(get_weapon_tag(), (unsigned long) get_player_weapon());
}

SCM __api_get_player_level()
{
	return scm_from_int(get_player_level());
}

SCM __api_hit_player(SCM dmg)
{
	hit_player(scm_to_int(dmg));
	return SCM_BOOL_F;
}

SCM __api_give_player_exp(SCM exp)
{
	give_player_exp(scm_to_double(exp));
	return SCM_BOOL_F;
}

SCM __api_warp_player(SCM x, SCM y)
{
	warp_player(scm_to_int(x), scm_to_int(y));
	return SCM_BOOL_F;
}

SCM __api_set_player_weapon_index(SCM i)
{
	set_player_weapon_index(scm_to_int(i));
	return SCM_BOOL_F;
}

SCM __api_shoot_player_weapon(SCM pressed, SCM xv, SCM yv)
{
	shoot_player_weapon(scm_to_int(pressed), scm_to_double(xv), scm_to_double(yv));
	return SCM_BOOL_F;
}

void initialize_player()
{
	PLAYER_HEALTH = PLAYER_MAX_HEALTH;

	PLAYER_RESOURCES[NORTH] = load_texture("textures/player/default/n.png", 0, 0);
	PLAYER_RESOURCES[SOUTH] = load_texture("textures/player/default/s.png", 0, 0);
	PLAYER_RESOURCES[WEST] = load_texture("textures/player/default/w.png", 0, 0);
	PLAYER_RESOURCES[EAST] = load_texture("textures/player/default/e.png", 0, 0);

	PLAYER_ALTERNATE[NORTH] = load_texture("textures/player/default/na.png", 0, 0);
	PLAYER_ALTERNATE[SOUTH] = load_texture("textures/player/default/sa.png", 0, 0);
	PLAYER_ALTERNATE[WEST] = load_texture("textures/player/default/wa.png", 0, 0);
	PLAYER_ALTERNATE[EAST] = load_texture("textures/player/default/ea.png", 0, 0);

	/*PLAYER_WEAPONS[1] = make_weapon(COLOR_RED, 8, 8, 16, 8, 100.0, 0, 1, 8, "sfx/laser.wav");
	PLAYER_WEAPONS[1]->x = &PLAYER_X;
	PLAYER_WEAPONS[1]->y = &PLAYER_Y;
	PLAYER_WEAPONS[2] = make_weapon(COLOR_BLUE, 8, 8, 16, 2, 100.0, 1, 1, 8, "sfx/beam.wav");
	PLAYER_WEAPONS[2]->x = &PLAYER_X;
	PLAYER_WEAPONS[2]->y = &PLAYER_Y;*/

	scm_c_define_gsubr("get-player-x", 0, 0, 0, __api_get_player_x);
	scm_c_define_gsubr("get-player-y", 0, 0, 0, __api_get_player_y);
	scm_c_define_gsubr("get-player-w", 0, 0, 0, __api_get_player_w);
	scm_c_define_gsubr("get-player-h", 0, 0, 0, __api_get_player_h);
	scm_c_define_gsubr("get-player-facing", 0, 0, 0, __api_get_player_facing);
	scm_c_define_gsubr("get-player-health", 0, 0, 0, __api_get_player_health);
	scm_c_define_gsubr("get-player-max-health", 0, 0, 0, __api_get_player_max_health);
	scm_c_define_gsubr("get-player-charge-percent", 0, 0, 0, __api_get_player_charge_percent);
	scm_c_define_gsubr("get-player-exp", 0, 0, 0, __api_get_player_exp);
	scm_c_define_gsubr("get-player-exp-to-next", 0, 0, 0, __api_get_player_exp_to_next);
	scm_c_define_gsubr("set-player-weapon", 2, 0, 0, __api_set_player_weapon);
	scm_c_define_gsubr("get-player-weapon", 0, 0, 0, __api_get_player_weapon);
	scm_c_define_gsubr("get-player-level", 0, 0, 0, __api_get_player_level);
	scm_c_define_gsubr("hit-player", 1, 0, 0, __api_hit_player);
	scm_c_define_gsubr("give-player-exp", 1, 0, 0, __api_give_player_exp);
	scm_c_define_gsubr("warp-player", 2, 0, 0, __api_warp_player);
	scm_c_define_gsubr("set-player-weapon-index", 1, 0, 0, __api_set_player_weapon_index);
	scm_c_define_gsubr("shoot-player-weapon", 3, 0, 0, __api_shoot_player_weapon);

	scm_c_primitive_load("init/player.scm");
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

double get_player_charge_percent()
{
	return PLAYER_WEAPONS[PLAYER_WEAPON_INDEX]->charge;
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

void set_player_weapon(int i, weapon *w)
{
	PLAYER_WEAPONS[i] = w;
	w->x = &PLAYER_X;
	w->y = &PLAYER_Y;
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

void shoot_player_weapon(int pressed, double xv, double yv)
{
	if (pressed) {
		press_trigger(PLAYER_WEAPONS[PLAYER_WEAPON_INDEX], xv, yv);
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

void update_player()
{
	if (PLAYER_HEALTH <= 0.0) {
		set_mode(GAME_OVER_MODE);
		return;
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

	int xmin = (PLAYER_X/TILE_DIM)-2;
	int ymin = (PLAYER_Y/TILE_DIM)-2;
	int xmax = (PLAYER_X/TILE_DIM)+2;
	int ymax = (PLAYER_Y/TILE_DIM)+2;
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		for (y = ymin; y <= ymax; y++) {
			if (is_solid_tile(x, y)) {
				b.x = x*32;
				b.y = y*32;
				b.w = b.h = TILE_DIM;
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
}

void draw_player()
{
	if (PLAYER_ANIM_STEP < PLAYER_ANIM_STEP_TIMING / 2) {
		draw_texture(PLAYER_RESOURCES[PLAYER_FACING], PLAYER_X, PLAYER_Y);
	} else {
		draw_texture(PLAYER_ALTERNATE[PLAYER_FACING], PLAYER_X, PLAYER_Y);
	}
}
