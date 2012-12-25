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
		shoot_enemy_weapon(e, random() % 4);
	}
}

void ai_cb_amble(enemy *e)
{
	if (!e->xv && !e->yv) {
		direction d = random() % 8;
		switch (d) {
			case NORTH:
				e->yv = -e->speed;
				break;
			case SOUTH:
				e->yv = e->speed;
				break;
			case WEST:
				e->xv = -e->speed;
				break;
			case EAST:
				e->xv = e->speed;
				break;
			default:
				break;
		}
	} else {
		if (e->yv < 0) {
			e->yv += 1;
		} else {
			e->yv -= 1;
		}
		if (e->xv < 0) {
			e->xv += 1;
		} else {
			e->xv -= 1;
		}
	}
	ai_cb_sentry(e);
	SDL_Rect a = {
		e->x + e->xv,
		e->y + e->yv,
		e->w,
		e->h
	};
	move_enemy_worker(e, a);
}

void ai_cb_track(enemy *e)
{
	;
}

void ai_cb_sentry(enemy *e)
{
	int px = get_player_x();
	int py = get_player_y();
	if (py <= e->y && px <= e->x + 50 && px >= e->x - 50) {
		shoot_enemy_weapon(e, NORTH);
	} else if (py >= e->y && px <= e->x + 50 && px >= e->x - 50) {
		shoot_enemy_weapon(e, SOUTH);
	} else if (px <= e->x && py <= e->y + 50 && py >= e->y - 50) {
		shoot_enemy_weapon(e, WEST);
	} else if (px >= e->x && py <= e->y + 50 && py >= e->y - 50) {
		shoot_enemy_weapon(e, EAST);
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
	set_ai_handler(1, AI_AMBLE);
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
