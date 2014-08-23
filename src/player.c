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

static hash_map *PLAYER_STATS;

static double PLAYER_X = 0;
static double PLAYER_Y = 0;
static bool PLAYER_PRESSED[4] = {0, 0, 0, 0};
static direction PLAYER_FACING = 0;
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

SCM __api_get_player_stat(SCM k)
{
	char *s = scm_to_locale_string(k);
	return scm_from_double(get_player_stat(s));
	free(s);
}

SCM __api_set_player_stat(SCM k, SCM v)
{
	char *s = scm_to_locale_string(k);
	set_player_stat(s, scm_to_double(v));
	free(s);
	return SCM_BOOL_F;
}

SCM __api_get_player_item()
{
	return scm_new_smob(get_item_tag(), (unsigned long) get_player_item());
}

SCM __api_set_player_item(SCM i, SCM w)
{
	item *weap = (item *) SCM_SMOB_DATA(w);
	set_player_item(scm_to_int(i), weap);
	return SCM_BOOL_F;
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
	PLAYER_STATS = make_hash_map();
	PLAYER_TEXTURE = load_texture("textures/player.png", 8, 8);

	scm_c_define_gsubr("get-player-x", 0, 0, 0, __api_get_player_x);
	scm_c_define_gsubr("get-player-y", 0, 0, 0, __api_get_player_y);
	scm_c_define_gsubr("get-player-stat", 1, 0, 0, __api_get_player_stat);
	scm_c_define_gsubr("set-player-stat", 2, 0, 0, __api_set_player_stat);
	scm_c_define_gsubr("set-player-item", 2, 0, 0, __api_set_player_item);
	scm_c_define_gsubr("get-player-item", 0, 0, 0, __api_get_player_item);
	scm_c_define_gsubr("hit-player", 1, 0, 0, __api_hit_player);
	scm_c_define_gsubr("give-player-exp", 1, 0, 0, __api_give_player_exp);
	scm_c_define_gsubr("warp-player", 2, 0, 0, __api_warp_player);
	scm_c_define_gsubr("set-player-item-index", 1, 0, 0, __api_set_player_item_index);
	scm_c_define_gsubr("use-player-item", 2, 0, 0, __api_use_player_item);

	load_all("script/players");
}

void reset_player()
{
	
	memset(PLAYER_PRESSED, 0, sizeof(PLAYER_PRESSED));
	PLAYER_X = PLAYER_Y = 0;
	PLAYER_FACING = 0;
	PLAYER_ANIM_STEP = 0;
	PLAYER_ITEM_INDEX = 0;

	load_all("script/players");
}

double get_player_x()
{
	return PLAYER_X;
}

double get_player_y()
{
	return PLAYER_Y;
}

double get_player_stat(char *k)
{
	double *v;
	if ((v = get_hash(PLAYER_STATS, k)) == NULL) {
		log_err("Player stat \"%s\" does not exist", k);
		exit(1);
	} else {
		return *v;
	}
}

void set_player_stat(char *k, double v)
{
	double *p;
	if ((p = get_hash(PLAYER_STATS, k)) == NULL) {
		p = (double *) malloc(sizeof(double));
		*p = v;
		set_hash(PLAYER_STATS, k, p);
	} else {
		*p = v;
	}
}

item *get_player_item()
{
	return PLAYER_ITEMS[PLAYER_ITEM_INDEX];
}

void set_player_item(int i, item *it)
{
	PLAYER_ITEMS[i] = it;
	it->x = &PLAYER_X;
	it->y = &PLAYER_Y;
}

void hit_player(int dmg)
{
	set_player_stat("health", get_player_stat("health") - dmg);
}

void give_player_exp(double exp)
{
	double cur = get_player_stat("exp");
	double next = get_player_stat("exp_to_next");
	cur += exp;
	if (cur >= next) {
		set_player_stat("exp", cur - next);
		next += 50;
		set_player_stat("level", get_player_stat("level") + 1);
	} else {
		set_player_stat("exp", cur);
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
	double health = get_player_stat("health");
	double max_health = get_player_stat("max_health");
	double regen = get_player_stat("regen");
	if (health <= 0.0) {
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

	if (health <= max_health - regen) {
		set_player_stat("health", health + regen);
	} else if (health < max_health) {
		set_player_stat("health", max_health);
	}

	int tempx = PLAYER_X;
	int tempy = PLAYER_Y;

	double move_speed = get_player_stat("move_speed");
	double diag_speed = get_player_stat("diag_speed");

	if (PLAYER_PRESSED[DIR_NORTH]) {
		if (PLAYER_PRESSED[DIR_WEST] || PLAYER_PRESSED[DIR_EAST]) {
			tempy -= diag_speed;
		} else {
			tempy -= move_speed;
		}
	}

	if (PLAYER_PRESSED[DIR_SOUTH]) {
		if (PLAYER_PRESSED[DIR_WEST] || PLAYER_PRESSED[DIR_EAST]) {
			tempy += diag_speed;
		} else {
			tempy += move_speed;
		}
	}

	if (PLAYER_PRESSED[DIR_WEST]) {
		if (PLAYER_PRESSED[DIR_NORTH] || PLAYER_PRESSED[DIR_SOUTH]) {
			tempx -= diag_speed;
		} else {
			tempx -= move_speed;
		}
	}

	if (PLAYER_PRESSED[DIR_EAST]) {
		if (PLAYER_PRESSED[DIR_NORTH] || PLAYER_PRESSED[DIR_SOUTH]) {
			tempx += diag_speed;
		} else {
			tempx += move_speed;
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
		get_player_stat("width"),
		get_player_stat("height")
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
