#include "gui.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <libguile.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

#include "utils.h"
#include "texture.h"
#include "level.h"
#include "player.h"

static texture *BUTTON_BACKGROUND = NULL;
static texture *DIALOG_BOX_BACKGROUND = NULL;
static texture *METER_BACKGROUND = NULL;

static list_node *GUI_ELEMENTS;

static texture *BITMAP_FONT = NULL;

SCM __api_reset_gui()
{
	reset_gui();
	return SCM_BOOL_F;
}

SCM __api_spawn_image(SCM x, SCM y, SCM path)
{
	char *s = scm_to_locale_string(path);
	spawn_image(scm_to_double(x), scm_to_double(y), s);
	free(s);
	return SCM_BOOL_F;
}

SCM __api_spawn_label(SCM x, SCM y, SCM text, SCM size)
{
	char *s = scm_to_locale_string(text);
	spawn_label(scm_to_double(x), scm_to_double(y), s, scm_to_double(size));
	free(s);
	return SCM_BOOL_F;
}

SCM __api_spawn_dynamic_label(SCM x, SCM y, SCM getter, SCM size)
{
	spawn_dynamic_label(scm_to_double(x), scm_to_double(y), getter, scm_to_double(size));
	return SCM_BOOL_F;
}

SCM __api_spawn_button(SCM x, SCM y, SCM text, SCM click_callback)
{
	char *s = scm_to_locale_string(text);
	spawn_button(scm_to_double(x), scm_to_double(y), s, click_callback);
	free(s);
	return SCM_BOOL_F;
}

SCM __api_spawn_dialog_box(SCM x, SCM y, SCM text)
{
	char *s = scm_to_locale_string(text);
	spawn_dialog_box(scm_to_double(x), scm_to_double(y), s);
	free(s);
	return SCM_BOOL_F;
}

SCM __api_spawn_meter(SCM x, SCM y, SCM text, SCM col, SCM full_getter)
{
	char *s = scm_to_locale_string(text);
	color c = *((color *) SCM_SMOB_DATA(col));
	spawn_meter(scm_to_double(x), scm_to_double(y), s, c, full_getter);
	free(s);
	return SCM_BOOL_F;
}

void initialize_gui()
{
	BUTTON_BACKGROUND = load_texture("textures/gui/buttontemplate.png", 0, 0);
	DIALOG_BOX_BACKGROUND = load_texture("textures/gui/dialogtemplate.png", 0, 0);
	METER_BACKGROUND = load_texture("textures/gui/meter.png", 0, 0);
	BITMAP_FONT = load_texture("fonts/font.png", 8, 8);
	GUI_ELEMENTS = make_list();

	scm_c_define_gsubr("reset-gui", 0, 0, 0, __api_reset_gui);
	scm_c_define_gsubr("spawn-image", 3, 0, 0, __api_spawn_image);
	scm_c_define_gsubr("spawn-label", 4, 0, 0, __api_spawn_label);
	scm_c_define_gsubr("spawn-dynamic-label", 4, 0, 0, __api_spawn_dynamic_label);
	scm_c_define_gsubr("spawn-button", 4, 0, 0, __api_spawn_button);
	scm_c_define_gsubr("spawn-dialog-box", 3, 0, 0, __api_spawn_dialog_box);
	scm_c_define_gsubr("spawn-meter", 5, 0, 0, __api_spawn_meter);
}

void reset_gui()
{
	list_node *c;
	for (c = GUI_ELEMENTS->next; c->next != NULL; c = c->next, free(c->prev)) {
		if (((gui_element *) c->data) != NULL) {
			free((gui_element *) c->data);
		}
	}
	GUI_ELEMENTS = make_list();
}

int bitmap_index(char c)
{
	if (isalpha(c)) {
		c = toupper(c);
		return c - 65;
	} else if (isdigit(c)) {
		return (c - 48) + 26;
	} else if (isspace(c)) {
		return 51;
	} else if (ispunct(c)) {
		switch (c) {
			case ',':
				return 39;
				break;
			case '\'':
				return 40;
				break;
			case '"':
				return 41;
				break;
			case ':':
				return 42;
				break;
			case ';':
				return 43;
				break;
			case '-':
				return 44;
				break;
			case '/':
				return 45;
				break;
			case '[':
				return 46;
				break;
			case ']':
				return 47;
				break;
			case '(':
				return 48;
				break;
			case ')':
				return 49;
				break;
			case '*':
				return 50;
				break;
		}
	}
	return 51;
}

void spawn_image(double x, double y, char *path)
{
	gui_element *ret = (gui_element *) malloc(sizeof(gui_element));
	ret->type = IMAGE;
	ret->data.image.image = load_texture(path, 0, 0);
	ret->x = x;
	ret->y = y;
	ret->w = ret->data.image.image->w;
	ret->h = ret->data.image.image->h;
	insert_list(GUI_ELEMENTS, (void *) ret);
}

void spawn_label(double x, double y, char *text, double size)
{
	gui_element *ret = (gui_element *) malloc(sizeof(gui_element));
	ret->type = LABEL;
	strncpy(ret->data.label.text, text, 256);
	ret->data.label.size = size;
	ret->x = x;
	ret->y = y;
	ret->w = strlen(text) * 8 * size;
	ret->h = 8 * size;
	insert_list(GUI_ELEMENTS, (void *) ret);
}

void spawn_dynamic_label(double x, double y, SCM getter, double size)
{
	gui_element *ret = (gui_element *) malloc(sizeof(gui_element));
	ret->type = DYNAMIC_LABEL;
	ret->data.dynamic_label.text_getter = getter;
	scm_gc_protect_object(ret->data.dynamic_label.text_getter);
	ret->data.dynamic_label.size = size;
	ret->x = x;
	ret->y = y;
	ret->w = 8 * size; //only use first character, it doesn't matter anyway
	ret->h = 8 * size;
	insert_list(GUI_ELEMENTS, (void *) ret);
}

void spawn_button(double x, double y, char *text, SCM click_callback)
{
	gui_element *ret = (gui_element *) malloc(sizeof(gui_element));
	ret->type = BUTTON;
	strncpy(ret->data.button.text, text, 256);
	ret->data.button.click_callback = click_callback;
	scm_gc_protect_object(ret->data.button.click_callback);
	ret->x = x;
	ret->y = y;
	ret->w = 200;
	ret->h = 50;
	insert_list(GUI_ELEMENTS, (void *) ret);
}

void spawn_dialog_box(double x, double y, char *text)
{
	gui_element *ret = (gui_element *) malloc(sizeof(gui_element));
	ret->type = DIALOG_BOX;
	strncpy(ret->data.dialog_box.text, text, 1024);
	ret->x = x;
	ret->y = y;
	ret->w = 1000;
	ret->h = 200;
	insert_list(GUI_ELEMENTS, (void *) ret);
}

void spawn_meter(double x, double y, char *text, color c, SCM full_getter)
{
	gui_element *ret = (gui_element *) malloc(sizeof(gui_element));
	ret->type = METER;
	strncpy(ret->data.meter.text, text, 256);
	ret->data.meter.c = c;
	ret->data.meter.full_getter = full_getter;
	scm_gc_protect_object(ret->data.meter.full_getter);
	ret->x = x;
	ret->y = y;
	ret->w = 110;
	ret->h = 32;
	insert_list(GUI_ELEMENTS, (void *) ret);
}

void mouse_clicked(double x, double y)
{
	list_node *c;
	gui_element *ge;
	SDL_Rect mousecursor = {x, y, 1, 1};
	SDL_Rect button;
	for (c = GUI_ELEMENTS->next; c->next != NULL; c = c->next) {
		if (((gui_element *) c->data) != NULL) {
			ge = (gui_element *) c->data;
			if (ge->type == BUTTON) {
				button.x = ge->x;
				button.y = ge->y;
				button.w = ge->w;
				button.h = ge->h;
				if (!check_collision(mousecursor, button)) {
					scm_call_0(ge->data.button.click_callback);
				}
			}
		}
	}
}

void render_text_bitmap(char *text, double x, double y, double size)
{
	int len = strlen(text);
	int bmpx = 0, bmpy = 0;
	double curx = x;
	double cury = y;
	int c;
	for (c = 0; c < len; c++) {
		bmpx = bmpy = 0;
		bmpx = bitmap_index(text[c]);
		if (text[c] == '\n') {
			curx = x;
			cury += 8 * size;
		} else {
			while (bmpx >= 13) {
				bmpx -= 13;
				bmpy += 1;
			}
			set_sheet_row(BITMAP_FONT, bmpy);
			set_sheet_column(BITMAP_FONT, bmpx);
			draw_texture_scale(BITMAP_FONT, curx, cury, 8 * size, 8 * size);
			curx += 8 * size;
		}
	}
}

void draw_image(gui_element *ge)
{
	draw_texture(ge->data.image.image, ge->x, ge->y);
}

void draw_label(gui_element *ge)
{
	render_text_bitmap(ge->data.label.text, ge->x, ge->y, ge->data.label.size);
}

void draw_dynamic_label(gui_element *ge)
{
	char *t = scm_to_locale_string(scm_call_0(ge->data.dynamic_label.text_getter));
	render_text_bitmap(t, ge->x, ge->y, ge->data.dynamic_label.size);
	free(t);
}

void draw_button(gui_element *ge)
{
	draw_texture(BUTTON_BACKGROUND, ge->x, ge->y);

	render_text_bitmap(ge->data.button.text, ge->x + 10, ge->y + 10, 2);
}

void draw_dialog_box(gui_element *ge)
{
	draw_texture(DIALOG_BOX_BACKGROUND, ge->x, ge->y);

	render_text_bitmap(ge->data.dialog_box.text, ge->x + 10, ge->y + 10, 2);
}

void draw_meter(gui_element *ge)
{
	draw_texture(METER_BACKGROUND, ge->x, ge->y);

	glPushMatrix();

	glTranslatef(ge->x + 4, ge->y + 4, 0);
	glScalef(scm_to_double(scm_call_0(ge->data.meter.full_getter)), 24, 1);
	glColor3f(ge->data.meter.c.r, ge->data.meter.c.g, ge->data.meter.c.b);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, get_standard_vertices_handler());
	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)(sizeof(GLfloat)*2));
	glVertexPointer(2, GL_FLOAT, sizeof(vertex), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_standard_indices_handler());
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();

	render_text_bitmap(ge->data.meter.text, ge->x + 55 - (16 * strlen(ge->data.meter.text))/2, ge->y + 8, 2);
}

void draw_gui_element(gui_element *ge)
{
	switch (ge->type) {
		case IMAGE:
			draw_image(ge);
			break;
		case LABEL:
			draw_label(ge);
			break;
		case DYNAMIC_LABEL:
			draw_dynamic_label(ge);
			break;
		case BUTTON:
			draw_button(ge);
			break;
		case DIALOG_BOX:
			draw_dialog_box(ge);
			break;
		case METER:
			draw_meter(ge);
			break;
	}
}

void draw_gui()
{
	list_node *c;
	for (c = GUI_ELEMENTS->next; c->next != NULL; c = c->next) {
		if (((gui_element *) c->data) != NULL) {
			draw_gui_element((gui_element *) c->data);
		}
	}
}
