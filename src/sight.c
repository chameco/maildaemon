#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <GL/glew.h>
#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "level.h"
#include "player.h"
#include "sight.h"

static list_node *CORNERS;
static ray RAYS[100];
static int RAY_INDEX = 0;

void initialize_sight()
{
	CORNERS = make_list();
	memset(RAYS, 0, sizeof(RAYS));
	RAY_INDEX = 0;
	//find_corners();
}

void reset_sight()
{
	list_node *c;
	for (c = CORNERS->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((line_point *) c->data) != NULL) {
			free((line_point *) c->data);
		}
	}
	CORNERS = make_list();
	memset(RAYS, 0, sizeof(RAYS));
	RAY_INDEX = 0;
	find_corners();
}

void find_corners()
{
	line_point *p;
	/*for (int x = 0; x < LEVEL_MAX_DIM; ++x) {
		for (int y = 0; y < LEVEL_MAX_DIM; ++y) {
			if (is_solid_tile(x, y)) {
				if (!is_solid_tile(x-1, y) && !is_solid_tile(x, y-1)) {
					p = (line_point *) malloc(sizeof(line_point));
					p->x = (x) * TILE_DIM;
					p->y = (y) * TILE_DIM;
					insert_list(CORNERS, p);
				}
				if (!is_solid_tile(x-1, y) && !is_solid_tile(x, y+1)) {
					p = (line_point *) malloc(sizeof(line_point));
					p->x = (x) * TILE_DIM;
					p->y = (y + 1) * TILE_DIM;
					insert_list(CORNERS, p);
				}
				if (!is_solid_tile(x+1, y) && !is_solid_tile(x, y-1)) {
					p = (line_point *) malloc(sizeof(line_point));
					p->x = (x + 1) * TILE_DIM;
					p->y = (y) * TILE_DIM;
					insert_list(CORNERS, p);
				}
				if (!is_solid_tile(x+1, y) && !is_solid_tile(x, y+1)) {
					p = (line_point *) malloc(sizeof(line_point));
					p->x = (x + 1) * TILE_DIM;
					p->y = (y + 1) * TILE_DIM;
					insert_list(CORNERS, p);
				}
			}
		}
	}*/
	p = (line_point *) malloc(sizeof(line_point));
	p->x = (14 + 0.5) * TILE_DIM;
	p->y = (0 + 0.5) * TILE_DIM;
	insert_list(CORNERS, p);

	p = (line_point *) malloc(sizeof(line_point));
	p->x = (14 + 0.5) * TILE_DIM;
	p->y = (9 + 0.5) * TILE_DIM;
	insert_list(CORNERS, p);

	/*p = (line_point *) malloc(sizeof(line_point));
	p->x = (25 + 0.5) * TILE_DIM;
	p->y = (0 + 0.5) * TILE_DIM;
	insert_list(CORNERS, p);*/

	p = (line_point *) malloc(sizeof(line_point));
	p->x = (25 + 0.5) * TILE_DIM;
	p->y = (9 + 0.5) * TILE_DIM;
	insert_list(CORNERS, p);
}

bool get_intersection(line ray, line segment, line_point *inter, double *param)
{
	double r_px, r_py, r_dx, r_dy;
	double s_px, s_py, s_dx, s_dy;
	r_px = ray.p1.x;
	r_py = ray.p1.y;
	r_dx = ray.p2.x - ray.p1.x;
	r_dy = ray.p2.y - ray.p1.y;

	s_px = segment.p1.x;
	s_py = segment.p1.y;
	s_dx = segment.p2.x - segment.p1.x;
	s_dy = segment.p2.y - segment.p1.y;

	double r_mag = sqrt(r_dx*r_dx+r_dy*r_dy);
	double s_mag = sqrt(s_dx*s_dx+s_dy*s_dy);
	if(r_dx/r_mag==s_dx/s_mag && r_dy/r_mag==s_dy/s_mag){
		return false;
	}
	double T2 = (r_dx*(s_py-r_py) + r_dy*(r_px-s_px))/(s_dx*r_dy - s_dy*r_dx);
	double T1 = (s_px+s_dx*T2-r_px)/r_dx;
	*param = T1;
	/*debug("r_px: %lf, r_py: %lf", r_px, r_py);
	debug("r_dx: %lf, r_dy: %lf", r_dx, r_dy);
	debug("s_px: %lf, s_py: %lf", s_px, s_py);
	debug("s_dx: %lf, s_dy: %lf", s_dx, s_dy);
	debug("T1: %lf, T2: %lf", T1, T2);*/

	if (T1<0) return false;
	if (T2<0 || T2>1) return false;

	inter->x = r_px+r_dx*T1;
	inter->y = r_py+r_dy*T1;
	return true;
}

void cast_ray(double mag, double theta, double far)
{
	line cast_ray, segment;
	line_point inter1, inter2;
	double param1, param2;
	bool b1 = false;
	bool b2 = false;
	double dx, dy;
	double sintheta = sin(theta);
	double costheta = cos(theta);

	cast_ray.p1.x = get_player_x();
	cast_ray.p1.y = get_player_y();
	cast_ray.p2.x = get_player_x() + mag * costheta;
	cast_ray.p2.y = get_player_y() + mag * sintheta;

	//TODO: Use actual segments here instead of test values
	segment.p1.x = (14 + 0.5) * TILE_DIM;
	segment.p1.y = (0 + 0.5) * TILE_DIM;
	segment.p2.x = (14 + 0.5) * TILE_DIM;
	segment.p2.y = (9 + 0.5) * TILE_DIM;
	b1 = get_intersection(cast_ray, segment, &inter1, &param1);

	/*segment.p1.x = (25 + 0.5) * TILE_DIM;
	segment.p1.y = (0 + 0.5) * TILE_DIM;
	segment.p2.x = (25 + 0.5) * TILE_DIM;
	segment.p2.y = (9 + 0.5) * TILE_DIM;
	b2 = get_intersection(cast_ray, segment, &inter2, &param2);*/

	segment.p1.x = (14 + 0.5) * TILE_DIM;
	segment.p1.y = (9 + 0.5) * TILE_DIM;
	segment.p2.x = (25 + 0.5) * TILE_DIM;
	segment.p2.y = (9 + 0.5) * TILE_DIM;
	b2 = get_intersection(cast_ray, segment, &inter2, &param2);
	//TODO END

	if (b1 && param1 <= param2) {
		RAYS[RAY_INDEX].l.p1 = cast_ray.p1;
		RAYS[RAY_INDEX].l.p2 = inter1;
	} else if (b2 && param2 < param1) {
		RAYS[RAY_INDEX].l.p1 = cast_ray.p1;
		RAYS[RAY_INDEX].l.p2 = inter2;
	} else {
		RAYS[RAY_INDEX].l.p1 = cast_ray.p1;
		RAYS[RAY_INDEX].l.p2.x = get_player_x() + (far ? 1000 : mag) * costheta;
		RAYS[RAY_INDEX].l.p2.y = get_player_y() + (far ? 1000 : mag) * sintheta;
	}
	dy = RAYS[RAY_INDEX].l.p2.y - RAYS[RAY_INDEX].l.p1.y;
	dx = RAYS[RAY_INDEX].l.p2.x - RAYS[RAY_INDEX].l.p1.x;
	RAYS[RAY_INDEX].theta = atan2(dy, dx);
	if (RAYS[RAY_INDEX].theta < 0) {
		RAYS[RAY_INDEX].theta = 2 * PI - RAYS[RAY_INDEX].theta;
	}
	RAY_INDEX++;
}

int thetacmp(const void *aa, const void *bb)
{
	ray *r1 = (ray *) aa;
	ray *r2 = (ray *) bb;
	double t1 = r1->theta;
	//if (t1 < 0) t1 = 2 * PI - t1;
	double t2 = r2->theta;
	//if (t2 < 0) t2 = 2 * PI - t2;
	return (t1 < t2) ? -1 : (t1 > t2);
}

void update_sight()
{
	list_node *c;
	line_point *p;
	memset(RAYS, 0, sizeof(RAYS));
	RAY_INDEX = 0;

	double dx, dy;
	for (c = CORNERS->next; c->next != NULL; c = c->next) {
		if (((line_point *) c->data) != NULL) {
			p = (line_point *) c->data;
			dx = p->x - get_player_x();
			dy = p->y - get_player_y();
			cast_ray(sqrt(dy*dy + dx*dx), atan2(dy, dx) - 0.01, false);
			cast_ray(sqrt(dy*dy + dx*dx), atan2(dy, dx), false);
			cast_ray(sqrt(dy*dy + dx*dx), atan2(dy, dx) + 0.01, false);
		}
	}
	qsort(RAYS, RAY_INDEX, sizeof(ray), thetacmp);
}

void draw_sight()
{
	for (int i = 0; i < RAY_INDEX; ++i) {
		glColor4f(1.0, 0.0, 0.0, 0.5);
		glBegin(GL_LINES);
		glVertex2f(get_player_x(), get_player_y());
		glVertex2f(RAYS[i].l.p2.x, RAYS[i].l.p2.y);
		glEnd();
	}
}
