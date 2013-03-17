#include "utils.h"

GLuint STANDARD_INDICES[4] = {0, 1, 2, 3};
GLuint STANDARD_INDICES_HANDLER = 0;

color COLOR_RED = {1.0, 0.0, 0.0, 1.0};
color COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
color COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
color COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
color COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
color COLOR_GRAY = {0.1, 0.1, 0.1, 1.0};

void initialize_utils()
{
	glGenBuffers(1, &STANDARD_INDICES_HANDLER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, STANDARD_INDICES_HANDLER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), STANDARD_INDICES, GL_STATIC_DRAW);
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
	list_node *end = (list_node *) malloc(sizeof(list_node));
	start->prev = NULL;
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
	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	SDL_PixelFormat *format = surface->format;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if (format->Amask) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4,
				surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	}
	else {
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
				surface->w, surface->h, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	}
	SDL_FreeSurface(surface);
	return texture;
}

GLuint load_texture(char *file)
{
	SDL_Surface *surface = IMG_Load(file);
	char buffer[100];
	if (surface == NULL) {
		strcpy(buffer, DATADIR);
		strcat(buffer, file);
		debug("%s", buffer);
		surface = IMG_Load(buffer);
	}
	return surface_to_texture(surface);
}

GLuint load_text(TTF_Font *font, char *text, int *w, int *h)
{
	SDL_Color white = {255, 255, 255};
	SDL_Surface *surface = TTF_RenderText_Blended(font, text, white);
	(*w) = surface->w;
	(*h) = surface->h;
	return surface_to_texture(surface);
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
