#ifndef AI_H
#define AI_H
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "utils.h"
#include "level.h"
#include "player.h"
#include "enemy.h"

typedef struct ai_handler {
	void (*callback)(enemy *);
} ai_handler;

ai_handler *get_ai_handler(int enemyid);
void set_ai_handler(int enemyid, ai_handler *ai);
ai_handler *make_ai_handler(void (*cb)(enemy *));
void ai_cb_jitter(enemy *e);
void ai_cb_amble(enemy *e);
void ai_cb_track(enemy *e);
void ai_cb_sentry(enemy *e);
void initialize_ai();
void update_ai();
#endif
