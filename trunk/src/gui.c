#include "gui.h"

GLuint HEALTHBAR;
GLuint MAGICBAR;
GLuint EXPBAR;
char CURRENT_PLAYER_LEVEL_TEXT[256];
char CURRENT_LEVEL_TEXT[256];
int BELIEVED_CURRENT_PLAYER_LEVEL = -1;
int BELIEVED_CURRENT_LEVEL = -1;
GLuint BUTTON_BACKGROUND;

void initialize_gui()
{
	HEALTHBAR = load_texture("textures/gui/health.png");
	MAGICBAR = load_texture("textures/gui/magic.png");
	EXPBAR = load_texture("textures/gui/exp.png");
	BUTTON_BACKGROUND = load_texture("textures/gui/buttontemplate.png");
}

void update_gui()
{
	;
}

void draw_button(int id, char *text, int x, int y)
{
	//debug("draw_button: id=%d, text=%s, x=%d, y=%d, BUTTON_TEXTURES[id]=%d",
			//id, text, x, y, BUTTON_TEXTURES[id]);
	glPushMatrix();
	glTranslatef(x, y, 0);

	glColor3f(1.0, 1.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, BUTTON_BACKGROUND);

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
	glTexCoord2i(1, 0); glVertex3f(200, 0, 0);
	glTexCoord2i(1, 1); glVertex3f(200, 50, 0);
	glTexCoord2i(0, 1); glVertex3f(0, 50, 0);
	glEnd();
	glPopMatrix();

	render_text_bitmap(x + 10, y + 10, text, 2);
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
	if (BELIEVED_CURRENT_PLAYER_LEVEL != get_player_level()) {
		BELIEVED_CURRENT_PLAYER_LEVEL = get_player_level();
		sprintf(CURRENT_PLAYER_LEVEL_TEXT, "level %d", BELIEVED_CURRENT_PLAYER_LEVEL);
	}
	if (BELIEVED_CURRENT_LEVEL != get_current_level_index()) {
		BELIEVED_CURRENT_LEVEL = get_current_level_index();
		char *buffer =  get_current_level_name();
		strncpy(CURRENT_LEVEL_TEXT, buffer, 256);
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

	render_text_bitmap(135, 72, CURRENT_PLAYER_LEVEL_TEXT, 1.5);

	//LEVEL TITLE
	
	render_text_bitmap(250, 0, CURRENT_LEVEL_TEXT, 4);

	glPopMatrix();
}
