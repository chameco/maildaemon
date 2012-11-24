#include "fx.h"

list_node *EFFECTS;

void initialize_effects()
{
	EFFECTS = make_list();
}

void spawn_effect(float r, float g, float b, float a,
		int radius, float duration)
{
	effect *e = (effect *) malloc(sizeof(effect));
	e->r = r;
	e->g = g;
	e->b = b;
	e->a = a;
	e->radius = radius;
	e->duration = duration;
	insert_list(EFFECTS, (void *) e);
}

void update_projectiles()
