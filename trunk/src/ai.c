#include "ai.h"

ai_handler *AI_HANDLERS[100];
ai_handler *AI_JITTER;
ai_handler *AI_AMBLE;
ai_handler *AI_TRACK;
ai_handler *AI_SENTRY;

ai_handler *get_ai_handler(int enemyid)
{
	return AI_HANDLERS[enemyid];
}

void set_ai_handler(int enemyid, ai_handler *ai)
{
	AI_HANDLERS[enemyid] = ai;
}

ai_handler *make_ai_handler(void (*cb)(enemy *)) {
	ai_handler *ret = (ai_handler *) malloc(sizeof(ai_handler));
	ret->callback = cb;
	return ret;
}

// AI DECLARATIONS //

void ai_cb_jitter(enemy *e)
{
	if ((random() % 11) > 5) {
		if ((random() % 11) > 5) {
			move_enemy_north(e);
		} else {
			move_enemy_south(e);
		}
	} else {
		if ((random() % 11) > 5) {
			move_enemy_west(e);
		} else {
			move_enemy_east(e);
		}
	}
	if (!(random() % 11)) {
		shoot_enemy_weapon(e, random() % 2, random() % 2);
	}
}

void ai_cb_amble(enemy *e)
{
	;
}

void ai_cb_track(enemy *e)
{
	int px = get_player_x();
	int py = get_player_y();
	int xdiff = px - e->x;
	int ydiff = py - e->y;
	if (abs(xdiff) > abs(ydiff)) {
		if (xdiff < 0) {
			move_enemy_west(e);
		} else {
			move_enemy_east(e);
		}
	} else {
		if (ydiff < 0) {
			move_enemy_north(e);
		} else {
			move_enemy_south(e);
		}
	}
	ai_cb_sentry(e);
}

void ai_cb_sentry(enemy *e)
{
	int px = get_player_x();
	int py = get_player_y();
	if (py <= e->y && px <= e->x + 50 && px >= e->x - 50) {
		shoot_enemy_weapon(e, 0, -1);
	} else if (py >= e->y && px <= e->x + 50 && px >= e->x - 50) {
		shoot_enemy_weapon(e, 0, 1);
	} else if (px <= e->x && py <= e->y + 50 && py >= e->y - 50) {
		shoot_enemy_weapon(e, -1, 0);
	} else if (px >= e->x && py <= e->y + 50 && py >= e->y - 50) {
		shoot_enemy_weapon(e, 1, 0);
	}
}

/////////////////////

void initialize_ai()
{
	AI_JITTER = make_ai_handler(ai_cb_jitter);
	AI_AMBLE = make_ai_handler(ai_cb_amble);
	AI_TRACK = make_ai_handler(ai_cb_track);
	AI_SENTRY = make_ai_handler(ai_cb_sentry);
	set_ai_handler(0, AI_SENTRY);
	set_ai_handler(1, AI_TRACK);
}

void update_ai()
{
	list_node *enemies = get_enemies();
	list_node *c;
	for (c = enemies->next; c->next != NULL; c = c->next) {
		if (((enemy *) c->data) != NULL) {
			get_ai_handler(((enemy *) c->data)->id)->callback(
					((enemy *) c->data));
		}
	}
}
