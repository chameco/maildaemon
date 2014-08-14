#include "player.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "texture.h"
#include "entity.h"
#include "item.h"
#include "game.h"
#include "level.h"

static double PLAYER_X = 0;
static double PLAYER_Y = 0;
static bool PLAYER_PRESSED[4] = {0, 0, 0, 0};
static const double PLAYER_MOVE_SPEED = 6; //Pixels per second
static const double PLAYER_DIAG_SPEED = 5;
static const int PLAYER_WIDTH = TILE_DIM;
static const int PLAYER_HEIGHT = (TILE_DIM)/2;
static direction PLAYER_FACING = 0;
static double PLAYER_EXP = 0;
static double PLAYER_EXP_TO_NEXT = 100;
static int PLAYER_LEVEL = 0;
static double PLAYER_MAX_HEALTH = 100;
static double PLAYER_HEALTH;
static double PLAYER_REGEN = 0.1;
static texture *PLAYER_TEXTURE;
static const int PLAYER_ANIM_STEP_TIMING = 8;
static int PLAYER_ANIM_STEP = 0;
static item *PLAYER_ITEMS[10] = {NULL};
static int PLAYER_ITEM_INDEX = 0;

SCM __api_get_player_x()
{
	return scm_from_double(get_player_x());
}

SCM __api_get_player_y()
{
	return scm_from_double(get_player_y());
}

SCM __api_get_player_w()
{
	return scm_from_int(get_player_w());
}

SCM __api_get_player_h()
{
	return scm_from_int(get_player_h());
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
	return scm_from_int(get_player_charge_percent());
}

SCM __api_get_player_exp()
{
	return scm_from_double(get_player_exp());
}

SCM __api_get_player_exp_to_next()
{
	return scm_from_double(get_player_exp_to_next());
}

SCM __api_set_player_item(SCM i, SCM w)
{
	item *weap = (item *) SCM_SMOB_DATA(w);
	set_player_item(scm_to_int(i), weap);
	return SCM_BOOL_F;
}

SCM __api_get_player_item()
{
	return scm_new_smob(get_item_tag(), (unsigned long) get_player_item());
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
	warp_player(scm_to_double(x), scm_to_double(y));
	return SCM_BOOL_F;
}

SCM __api_set_player_item_index(SCM i)
{
	set_player_item_index(scm_to_int(i));
	return SCM_BOOL_F;
}

SCM __api_use_player_item(SCM pressed, SCM rot)
{
	use_player_item(scm_to_int(pressed), scm_to_double(rot));
	return SCM_BOOL_F;
}

void initialize_player()
{
	PLAYER_HEALTH = PLAYER_MAX_HEALTH;

	PLAYER_TEXTURE = load_texture("textures/player.png", 8, 8);

	scm_c_define_gsubr("get-player-x", 0, 0, 0, __api_get_player_x);
	scm_c_define_gsubr("get-player-y", 0, 0, 0, __api_get_player_y);
	scm_c_define_gsubr("get-player-w", 0, 0, 0, __api_get_player_w);
	scm_c_define_gsubr("get-player-h", 0, 0, 0, __api_get_player_h);
	scm_c_define_gsubr("get-player-health", 0, 0, 0, __api_get_player_health);
	scm_c_define_gsubr("get-player-max-health", 0, 0, 0, __api_get_player_max_health);
	scm_c_define_gsubr("get-player-charge-percent", 0, 0, 0, __api_get_player_charge_percent);
	scm_c_define_gsubr("get-player-exp", 0, 0, 0, __api_get_player_exp);
	scm_c_define_gsubr("get-player-exp-to-next", 0, 0, 0, __api_get_player_exp_to_next);
	scm_c_define_gsubr("set-player-item", 2, 0, 0, __api_set_player_item);
	scm_c_define_gsubr("get-player-item", 0, 0, 0, __api_get_player_item);
	scm_c_define_gsubr("get-player-level", 0, 0, 0, __api_get_player_level);
	scm_c_define_gsubr("hit-player", 1, 0, 0, __api_hit_player);
	scm_c_define_gsubr("give-player-exp", 1, 0, 0, __api_give_player_exp);
	scm_c_define_gsubr("warp-player", 2, 0, 0, __api_warp_player);
	scm_c_define_gsubr("set-player-item-index", 1, 0, 0, __api_set_player_item_index);
	scm_c_define_gsubr("use-player-item", 2, 0, 0, __api_use_player_item);
}

void reset_player()
{
	
	memset(PLAYER_PRESSED, 0, sizeof(PLAYER_PRESSED));
	PLAYER_X = PLAYER_Y = 0;
	PLAYER_FACING = 0;
	PLAYER_EXP = 0;
	PLAYER_EXP_TO_NEXT = 100;
	PLAYER_LEVEL = 0;
	PLAYER_HEALTH = PLAYER_MAX_HEALTH;
	PLAYER_ANIM_STEP = 0;
	PLAYER_ITEM_INDEX = 0;
}

double get_player_x()
{
	return PLAYER_X;
}

double get_player_y()
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
	return PLAYER_ITEMS[PLAYER_ITEM_INDEX]->charge;
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

void set_player_item(int i, item *it)
{
	PLAYER_ITEMS[i] = it;
	it->x = &PLAYER_X;
	it->y = &PLAYER_Y;
}

item *get_player_item()
{
	return PLAYER_ITEMS[PLAYER_ITEM_INDEX];
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

void warp_player(double x, double y)
{
	PLAYER_X = x;
	PLAYER_Y = y;
}

void set_player_item_index(int i)
{
	deactivate_item(PLAYER_ITEMS[PLAYER_ITEM_INDEX]);
	if (PLAYER_ITEMS[i] != NULL) {
		PLAYER_ITEM_INDEX = i;
	}
}

void use_player_item(bool pressed, double rot)
{
	if (pressed) {
		activate_item(PLAYER_ITEMS[PLAYER_ITEM_INDEX], rot);
	} else {
		deactivate_item(PLAYER_ITEMS[PLAYER_ITEM_INDEX]);
	}
}

void set_player_movement(bool pressed, direction d)
{
	if (pressed) {
		PLAYER_PRESSED[d] = true;
		PLAYER_FACING = d;
		set_sheet_row(PLAYER_TEXTURE, d);
	} else {
		PLAYER_PRESSED[d] = false;
	}
}

void update_player()
{
	if (PLAYER_HEALTH <= 0.0) {
		set_mode("game_over");
		return;
	}

	if (PLAYER_PRESSED[DIR_NORTH] || PLAYER_PRESSED[DIR_SOUTH] || PLAYER_PRESSED[DIR_WEST] || PLAYER_PRESSED[DIR_EAST]) {
		PLAYER_ANIM_STEP = ((PLAYER_ANIM_STEP + 1) % PLAYER_ANIM_STEP_TIMING);
		if (PLAYER_ANIM_STEP == 0) {
			set_sheet_column(PLAYER_TEXTURE, (get_sheet_column(PLAYER_TEXTURE) % 2) + 1);
		}
	} else {
		set_sheet_column(PLAYER_TEXTURE, 0);
	}

	if (PLAYER_HEALTH <= PLAYER_MAX_HEALTH-PLAYER_REGEN) {
		PLAYER_HEALTH += PLAYER_REGEN;
	} else if (PLAYER_HEALTH < PLAYER_MAX_HEALTH) {
		PLAYER_HEALTH = PLAYER_MAX_HEALTH;
	}

	int tempx = PLAYER_X;
	int tempy = PLAYER_Y;

	if (PLAYER_PRESSED[DIR_NORTH]) {
		if (PLAYER_PRESSED[DIR_WEST] || PLAYER_PRESSED[DIR_EAST]) {
			tempy -= PLAYER_DIAG_SPEED;
		} else {
			tempy -= PLAYER_MOVE_SPEED;
		}
	}

	if (PLAYER_PRESSED[DIR_SOUTH]) {
		if (PLAYER_PRESSED[DIR_WEST] || PLAYER_PRESSED[DIR_EAST]) {
			tempy += PLAYER_DIAG_SPEED;
		} else {
			tempy += PLAYER_MOVE_SPEED;
		}
	}

	if (PLAYER_PRESSED[DIR_WEST]) {
		if (PLAYER_PRESSED[DIR_NORTH] || PLAYER_PRESSED[DIR_SOUTH]) {
			tempx -= PLAYER_DIAG_SPEED;
		} else {
			tempx -= PLAYER_MOVE_SPEED;
		}
	}

	if (PLAYER_PRESSED[DIR_EAST]) {
		if (PLAYER_PRESSED[DIR_NORTH] || PLAYER_PRESSED[DIR_SOUTH]) {
			tempx += PLAYER_DIAG_SPEED;
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
				b.x = x*TILE_DIM;
				b.y = y*TILE_DIM;
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
	draw_texture_scale(PLAYER_TEXTURE, PLAYER_X, PLAYER_Y - TILE_DIM/2, TILE_DIM, TILE_DIM);
}
