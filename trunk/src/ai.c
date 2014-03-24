#include "ai.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "gui.h"
#include "level.h"
#include "player.h"
#include "entity.h"

ai_handler *AI_HANDLERS[100];
ai_handler *AI_JITTER;
ai_handler *AI_AMBLE;
ai_handler *AI_TRACK;
ai_handler *AI_SENTRY;

ai_handler *get_ai_handler(int entityid)
{
	return AI_HANDLERS[entityid];
}

void set_ai_handler(int entityid, ai_handler *ai)
{
	AI_HANDLERS[entityid] = ai;
}

ai_handler *make_ai_handler(void (*cb)(entity *)) {
	ai_handler *ret = (ai_handler *) malloc(sizeof(ai_handler));
	ret->callback = cb;
	return ret;
}

// AI DECLARATIONS //

void ai_cb_jitter(entity *e)
{
	if ((rand() % 11) > 5) {
		if ((rand() % 11) > 5) {
			move_entity(e, NORTH);
		} else {
			move_entity(e, SOUTH);
		}
	} else {
		if ((rand() % 11) > 5) {
			move_entity(e, WEST);
		} else {
			move_entity(e, EAST);
		}
	}
	if (!(rand() % 11)) {
		press_trigger(e->weapon, rand() % 4);
	}
}

void ai_cb_amble(entity *e)
{
	;
}

void ai_cb_track(entity *e)
{
	int px = get_player_x();
	int py = get_player_y();
	int xdiff = px - e->x;
	int ydiff = py - e->y;
	if (xdiff == 0 || ydiff == 0) {
		ai_cb_sentry(e);
	} else {
		if (xdiff < 0) {
			e->xv = -e->speed;
		} else if (xdiff > 0) {
			e->xv = e->speed;
		}
		if (ydiff < 0) {
			e->yv = -e->speed;
		} else if (ydiff > 0) {
			e->yv = e->speed;
		}
	}
}

void ai_cb_sentry(entity *e)
{
	int px = get_player_x() + get_player_w()/2;
	int py = get_player_y() + get_player_h()/2;
	int ex = e->x + e->w/2;
	int ey = e->y + e->h/2;
	int xdiff = px - ex;
	int ydiff = py - ey;
	if (ydiff < 0 && px <= ex + 50 && px >= ex - 50) {
		press_trigger(e->weapon, NORTH);
	} else if (ydiff > 0 && px <= ex + 50 && px >= ex - 50) {
		press_trigger(e->weapon, SOUTH);
	} else if (xdiff < 0 && py <= ey + 50 && py >= ey - 50) {
		press_trigger(e->weapon, WEST);
	} else if (xdiff > 0 && py <= ey + 50 && py >= ey - 50) {
		press_trigger(e->weapon, EAST);
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
	set_ai_handler(2, AI_TRACK);
	set_ai_handler(3, AI_AMBLE);
}

void update_ai()
{
	list_node *entities = get_entities();
	list_node *c;
	for (c = entities->next; c->next != NULL; c = c->next) {
		if (((entity *) c->data) != NULL) {
			get_ai_handler(((entity *) c->data)->id)->callback(
					((entity *) c->data));
		}
	}
}
