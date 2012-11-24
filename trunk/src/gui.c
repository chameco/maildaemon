#include "gui.h"

GLuint HEALTHBAR;
GLuint MAGICBAR;
GLuint EXPBAR;
GLuint CURRENT_LEVEL_TEX;
int CURRENT_LEVEL_TEX_W;
int CURRENT_LEVEL_TEX_H;
TTF_Font *GAMEFONT;
int BELIEVED_CURRENT_LEVEL = 0;

void initialize_gui()
{
	HEALTHBAR = load_texture(DATADIR "textures/gui/health.png");
	MAGICBAR = load_texture(DATADIR "textures/gui/magic.png");
	EXPBAR = load_texture(DATADIR "textures/gui/exp.png");
	GAMEFONT = TTF_OpenFont(DATADIR "fonts/courier.ttf", 16);
	if(!GAMEFONT) {
		log_err("Failure to open font: %s", TTF_GetError());
	}
	char sprintf_fodder[32];
	sprintf(sprintf_fodder, "level %d", get_player_level());
	CURRENT_LEVEL_TEX = load_text(GAMEFONT, sprintf_fodder,
			&CURRENT_LEVEL_TEX_W, &CURRENT_LEVEL_TEX_H);
}

void update_gui()
{
	;
}

void draw_gui()
{
	double h = get_player_health();
	double mh = get_player_max_health();
	double m = get_player_magic();
	double mm = get_player_max_magic();
	double e = get_player_exp();
	double me = get_player_exp_to_next();
	int hlength;
	hlength = (100 * h) / mh;
	int mlength;
	mlength = (100 * m) / mm;
	int elength;
	elength = (100 * e) / me;
	if (BELIEVED_CURRENT_LEVEL != get_player_level()) {
		BELIEVED_CURRENT_LEVEL = get_player_level();
		char sprintf_fodder[32];
		sprintf(sprintf_fodder, "level %d", BELIEVED_CURRENT_LEVEL);
		CURRENT_LEVEL_TEX = load_text(GAMEFONT, sprintf_fodder,
				&CURRENT_LEVEL_TEX_W, &CURRENT_LEVEL_TEX_H);
	}

	glPushMatrix();
	glTranslatef(0, 0, 0);

	//HEALTH BAR
	glPushMatrix();

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, HEALTHBAR);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(256, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(256, 32, 0);
		glTexCoord2i(0, 1); glVertex3f(0, 32, 0);
	glEnd();

	glTranslatef(127, 4, 0);
	glColor3f(0.0, 1.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(hlength, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(hlength, 24, 0);
		glTexCoord2i(0, 1); glVertex3f(0, 24, 0);
	glEnd();

	glPopMatrix();

	//MAGIC BAR
	glPushMatrix();

	glTranslatef(0, 32, 0);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, MAGICBAR);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(256, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(256, 32, 0);
		glTexCoord2i(0, 1); glVertex3f(0, 32, 0);
	glEnd();

	glTranslatef(127, 4, 0);
	glColor3f(0.0, 0.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(mlength, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(mlength, 24, 0);
		glTexCoord2i(0, 1); glVertex3f(0, 24, 0);
	glEnd();

	glPopMatrix();

	//EXP BAR
	glPushMatrix();

	glTranslatef(0, 64, 0);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, EXPBAR);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(256, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(256, 32, 0);
		glTexCoord2i(0, 1); glVertex3f(0, 32, 0);
	glEnd();

	glTranslatef(127, 4, 0);
	glColor3f(0.8, 0.5, 0.0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(elength, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(elength, 24, 0);
		glTexCoord2i(0, 1); glVertex3f(0, 24, 0);
	glEnd();

	glPopMatrix();

	glPushMatrix();

	glTranslatef(140, 70, 0);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, CURRENT_LEVEL_TEX);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(CURRENT_LEVEL_TEX_W, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(CURRENT_LEVEL_TEX_W, CURRENT_LEVEL_TEX_H, 0);
		glTexCoord2i(0, 1); glVertex3f(0, CURRENT_LEVEL_TEX_H, 0);
	glEnd();

	glPopMatrix();

	glPopMatrix();
}
