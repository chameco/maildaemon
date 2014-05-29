#include "gui.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <libguile.h>

#include "cuttle/debug.h"
#include "cuttle/utils.h"

#include "utils.h"
#include "resources.h"
#include "level.h"
#include "player.h"

static resource *BUTTON_BACKGROUND;
static resource *DIALOG_BOX_BACKGROUND;
static resource *METER_BACKGROUND;
static char CURRENT_PLAYER_LEVEL_TEXT[256];
static char CURRENT_LEVEL_TEXT[256];
static int BELIEVED_CURRENT_PLAYER_LEVEL = -1;

SDL_Surface *BITMAP = NULL;
GLuint BITMAP_FONT[4][26] = {{0}};

SCM __api_render_text_bitmap(SCM x, SCM y, SCM text, SCM size)
{
	char *t = scm_to_locale_string(text);
	render_text_bitmap(scm_to_int(x), scm_to_int(y), t, scm_to_double(size));
	free(t);
	return SCM_BOOL_F;
}

SCM __api_draw_button(SCM text, SCM x, SCM y)
{
	char *t = scm_to_locale_string(text);
	draw_button(t, scm_to_int(x), scm_to_int(y));
	free(t);
	return SCM_BOOL_F;
}

SCM __api_draw_dialog_box(SCM text, SCM x, SCM y)
{
	char *t = scm_to_locale_string(text);
	draw_dialog_box(t, scm_to_int(x), scm_to_int(y));
	free(t);
	return SCM_BOOL_F;
}

SCM __api_draw_meter(SCM text, SCM x, SCM y, SCM c, SCM full)
{
	char *t = scm_to_locale_string(text);
	color *col = (color *) SCM_SMOB_DATA(c);
	draw_meter(t, scm_to_int(x), scm_to_int(y), *col, scm_to_int(full));
	free(t);
	return SCM_BOOL_F;
}

void initialize_gui()
{
	BUTTON_BACKGROUND = load_resource("textures/gui/buttontemplate.png", 0, 0);
	DIALOG_BOX_BACKGROUND = load_resource("textures/gui/dialogtemplate.png", 0, 0);
	METER_BACKGROUND = load_resource("textures/gui/meter.png", 0, 0);
	load_bitmap_font("fonts/font.png");

	scm_c_define_gsubr("render-text-bitmap", 4, 0, 0, __api_render_text_bitmap);
	scm_c_define_gsubr("draw-button", 3, 0, 0, __api_draw_button);
	scm_c_define_gsubr("draw-dialog-box", 3, 0, 0, __api_draw_dialog_box);
	scm_c_define_gsubr("draw-meter", 5, 0, 0, __api_draw_meter);
}

void load_bitmap_font(char *path)
{
	BITMAP = IMG_Load(path);
	//SDL_SetSurfaceAlphaMod(BITMAP, 0);
	SDL_SetSurfaceBlendMode(BITMAP, SDL_BLENDMODE_NONE);
	SDL_SetColorKey(BITMAP, SDL_TRUE, SDL_MapRGBA(BITMAP->format, 255, 0, 0, 255));
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif


	int x, y;
	SDL_Rect srcrect = {0, 0, 8, 8};
	SDL_Rect destrect = {0, 0, 0, 0};
	for (y = 0; y < 4; y++) {
		for (x = 0; x < 26; x++) {
			srcrect.x = srcrect.y = 0;
			srcrect.x = x * 8;
			srcrect.y = y * 8;
			SDL_Surface *temp = SDL_CreateRGBSurface(0, 8, 8, 32,
					rmask, gmask, bmask, amask);
			SDL_Surface *character = SDL_ConvertSurface(temp, BITMAP->format, BITMAP->flags);
			SDL_Rect all = {0, 0, 8, 8};
			SDL_FillRect(character, &all, SDL_MapRGBA(temp->format, 0, 0, 0, 0));
			SDL_FreeSurface(temp);
			SDL_BlitSurface(BITMAP, &srcrect, character, &destrect);
			BITMAP_FONT[y][x] = surface_to_texture(character);
		}
	}
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
			case '_':
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

void render_text_bitmap(int x, int y, char *text, double size)
{
	int len = strlen(text);
	int bmpx = 0, bmpy = 0;
	int curx = x;
	int cury = y;
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
			glPushMatrix();

			glTranslatef(curx, cury, 0);
			glScalef(size * 8, size * 8, 1);
			glColor3f(1.0, 1.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, BITMAP_FONT[bmpy][bmpx]);

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
			curx += 8 * size;
		}
	}
}

void update_gui()
{
	;
}

void draw_button(char *text, int x, int y)
{
	draw_resource(BUTTON_BACKGROUND, x, y);

	render_text_bitmap(x + 10, y + 10, text, 2);
}

void draw_dialog_box(char *text, int x, int y)
{
	draw_resource(DIALOG_BOX_BACKGROUND, x, y);

	render_text_bitmap(x + 10, y + 10, text, 2);
}

void draw_meter(char *text, int x, int y, color c, int full)
{
	draw_resource(METER_BACKGROUND, x, y);

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

	render_text_bitmap(x + 55 - (16 * strlen(text))/2, y + 8, text, 2);
}

void draw_gui()
{
	double h = get_player_health();
	double mh = get_player_max_health();
	double e = get_player_exp();
	double me = get_player_exp_to_next();
	int hlength;
	hlength = (100 * h) / mh;
	int elength;
	elength = (100 * e) / me;
	if (BELIEVED_CURRENT_PLAYER_LEVEL != get_player_level()) {
		BELIEVED_CURRENT_PLAYER_LEVEL = get_player_level();
		sprintf(CURRENT_PLAYER_LEVEL_TEXT, "lvl %d", BELIEVED_CURRENT_PLAYER_LEVEL);
	}
	if (strcmp(CURRENT_LEVEL_TEXT, get_current_level()->name) != 0) {
		char *buffer = get_current_level()->name;
		strncpy(CURRENT_LEVEL_TEXT, buffer, sizeof(CURRENT_LEVEL_TEXT));
	}

	draw_meter("Health", 0, 0, COLOR_GREEN, hlength);
	draw_meter("Ammo", 0, 32, COLOR_GRAY, get_player_charge_percent());
	draw_meter(CURRENT_PLAYER_LEVEL_TEXT, 0, 64, (color) {1.0, 0.8, 0.0, 1.0}, elength);

	//LEVEL TITLE
	render_text_bitmap(150, 0, CURRENT_LEVEL_TEXT, 4);
}
