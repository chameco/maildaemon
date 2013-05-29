#include "utils.h"

GLuint STANDARD_INDICES[4] = {0, 1, 2, 3};
GLuint STANDARD_INDICES_HANDLER = 0;

color COLOR_RED = {1.0, 0.0, 0.0, 1.0};
color COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
color COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
color COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
color COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
color COLOR_GRAY = {0.1, 0.1, 0.1, 1.0};

SDL_Surface *BITMAP = NULL;
GLuint BITMAP_FONT[4][26] = {{0}};

void initialize_utils()
{
	glGenBuffers(1, &STANDARD_INDICES_HANDLER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, STANDARD_INDICES_HANDLER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), STANDARD_INDICES, GL_STATIC_DRAW);
	load_bitmap_font("fonts/font.png");
}

GLuint get_standard_indices_handler()
{
	return STANDARD_INDICES_HANDLER;
}

list_node *make_node(void *d)
{
	list_node *ret = (list_node *) malloc(sizeof(list_node));
	ret->data = d;
	return ret;
}

list_node *make_list()
{
	list_node *start = (list_node *) malloc(sizeof(list_node));
	list_node *end = (list_node *) malloc(sizeof(list_node)); start->prev = NULL;
	start->next = end;
	start->data = NULL;
	end->prev = start;
	end->next = NULL;
	end->data = NULL;
	return start;
}

void insert_list(list_node *l, void *data)
{
	list_node *ins = (list_node *) malloc(sizeof(list_node));
	ins->data = data;
	ins->next = l->next;
	l->next = ins;
	ins->next->prev = ins;
	ins->prev = l;
}

void remove_list(list_node *l, void *data)
{
	list_node *c;
	for (c = l; c->next != NULL; c = c->next) {
		if (c->data == data) {
			c->prev->next = c->next;
			c->next->prev = c->prev;
		}
	}
}

GLuint surface_to_texture(SDL_Surface *surface)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int mode;
	if (surface->format->BytesPerPixel == 4) {
		if (surface->format->Rmask == 0x000000ff) {
			mode = GL_RGBA;
		} else {
			mode = GL_BGRA;
		}
	}
	else {
		if (surface->format->Rmask == 0x000000ff) {
			debug("no alpha");
			mode = GL_RGB;
		} else {
			mode = GL_BGR;
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0,
			mode, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);
	return texture;
}

GLuint load_texture(char *path)
{
	SDL_Surface *surface = IMG_Load(path);
	char buffer[100];
	if (surface == NULL) {
		strcpy(buffer, DATADIR);
		strcat(buffer, path);
		debug("%s", buffer);
		surface = IMG_Load(buffer);
	}
	return surface_to_texture(surface);
}

void load_bitmap_font(char *path)
{
	BITMAP = IMG_Load(path);
	char buffer[100];
	if (BITMAP == NULL) {
		strcpy(buffer, DATADIR);
		strcat(buffer, path);
		debug("%s", buffer);
		BITMAP = IMG_Load(buffer);
	}
	SDL_SetAlpha(BITMAP, 0, 0);
	SDL_SetColorKey(BITMAP, SDL_SRCCOLORKEY,
			SDL_MapRGBA(BITMAP->format, 255, 0, 0, 0));
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
			//SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE,
					//8, 8, 32, rmask, gmask, bmask, 0);
			//SDL_Surface *character = SDL_ConvertSurface(temp, BITMAP->format, BITMAP->flags);
			SDL_Surface *character = IMG_Load("textures/blank.png");
			SDL_SetAlpha(character, 0, 0);
			//SDL_FreeSurface(temp);
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
	int c;
	for (c = 0; c < len; c++) {
		bmpx = bmpy = 0;
		bmpx = bitmap_index(text[c]);
		while (bmpx >= 13) {
			bmpx -= 13;
			bmpy += 1;
		}
		glPushMatrix();

		glTranslatef(x + (c * 8 * size), y, 0);
		glScalef(size, size, 1);
		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, BITMAP_FONT[bmpy][bmpx]);

		glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2i(1, 0); glVertex3f(8, 0, 0);
		glTexCoord2i(1, 1); glVertex3f(8, 8, 0);
		glTexCoord2i(0, 1); glVertex3f(0, 8, 0);
		glEnd();

		glPopMatrix();
	}
}

int check_collision(SDL_Rect A, SDL_Rect B)
{

	return !(((A.x >= B.x && A.x < B.x + B.w)  ||
				(B.x >= A.x && B.x < A.x + A.w)) && 
			((A.y >= B.y && A.y < B.y + B.h)   ||
			 (B.y >= A.y && B.y < A.y + A.h)));
}

double calculate_distance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void mouse_coords(int x, int y, GLdouble *ox, GLdouble *oy)
{
	GLdouble oz;
	GLint viewport[4];
	GLdouble modelview[16], projection[16];
	GLfloat wx = x, wy, wz;
	glGetIntegerv(GL_VIEWPORT, viewport);
	y=viewport[3]-y;
	wy=y;
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
	gluUnProject(wx,wy,wz,modelview,projection,viewport, ox, oy, &oz);
}

int gcd(int ak, int am) {
	int k = ak;   
	int m = am;
	while (k != m) {
		if (k > m) {
			k = k-m;
		}
		else { 
			m = m-k;
		}
	}
	return k;
}
