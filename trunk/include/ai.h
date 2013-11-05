#ifndef PURGE_AI_H
#define PURGE_AI_H
#include "entity.h"

typedef struct ai_handler {
	void (*callback)(entity *);
} ai_handler;

char *get_current_dialog();
void set_current_dialog(char *text);
ai_handler *get_ai_handler(int entityid);
void set_ai_handler(int entityid, ai_handler *ai);
ai_handler *make_ai_handler(void (*cb)(entity *));
void ai_cb_jitter(entity *e);
void ai_cb_amble(entity *e);
void ai_cb_track(entity *e);
void ai_cb_sentry(entity *e);
void initialize_ai();
void update_ai();
#endif
