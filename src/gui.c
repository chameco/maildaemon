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

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "texture.h"
#include "level.h"
#include "player.h"

static texture *BUTTON_BACKGROUND = NULL;
static texture *DIALOG_BOX_BACKGROUND = NULL;
static texture *METER_BACKGROUND = NULL;

static texture *BITMAP_FONT = NULL;

SCM __api_render_text_bitmap(SCM text, SCM x, SCM y, SCM size)
{
	char *t = scm_to_locale_string(text);
	render_text_bitmap(t, scm_to_double(x), scm_to_double(y), scm_to_double(size));
	free(t);
	return SCM_BOOL_F;
}

SCM __api_draw_button(SCM text, SCM x, SCM y)
{
	char *t = scm_to_locale_string(text);
	draw_button(t, scm_to_double(x), scm_to_double(y));
	free(t);
	return SCM_BOOL_F;
}

SCM __api_draw_dialog_box(SCM text, SCM x, SCM y)
{
	char *t = scm_to_locale_string(text);
	draw_dialog_box(t, scm_to_double(x), scm_to_double(y));
	free(t);
	return SCM_BOOL_F;
}

SCM __api_draw_meter(SCM text, SCM x, SCM y, SCM c, SCM full)
{
	char *t = scm_to_locale_string(text);
	color *col = (color *) SCM_SMOB_DATA(c);
	draw_meter(t, scm_to_double(x), scm_to_double(y), *col, scm_to_double(full));
	free(t);
	return SCM_BOOL_F;
}

void initialize_gui()
{
	BUTTON_BACKGROUND = load_texture("textures/gui/buttontemplate.png", 0, 0);
	DIALOG_BOX_BACKGROUND = load_texture("textures/gui/dialogtemplate.png", 0, 0);
	METER_BACKGROUND = load_texture("textures/gui/meter.png", 0, 0);
	BITMAP_FONT = load_texture("fonts/font.png", 8, 8);

	scm_c_define_gsubr("render-text-bitmap", 4, 0, 0, __api_render_text_bitmap);
	scm_c_define_gsubr("draw-button", 3, 0, 0, __api_draw_button);
	scm_c_define_gsubr("draw-dialog-box", 3, 0, 0, __api_draw_dialog_box);
	scm_c_define_gsubr("draw-meter", 5, 0, 0, __api_draw_meter);
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

void draw_button(char *text, double x, double y)
{
	draw_texture(BUTTON_BACKGROUND, x, y);

	render_text_bitmap(text, x + 10, y + 10, 2);
}

void draw_dialog_box(char *text, double x, double y)
{
	draw_texture(DIALOG_BOX_BACKGROUND, x, y);

	render_text_bitmap(text, x + 10, y + 10, 2);
}

void draw_meter(char *text, double x, double y, color c, double full)
{
	draw_texture(METER_BACKGROUND, x, y);

	glPushMatrix();

	glTranslatef(x + 4, y + 4, 0);
	glScalef(full, 24, 1);
	glColor3f(c.r, c.g, c.b);
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

	render_text_bitmap(text, x + 55 - (16 * strlen(text))/2, y + 8, 2);
}
