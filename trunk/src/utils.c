#include "utils.h"

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

GLuint load_texture(char *file)
{
	SDL_Surface* surface = IMG_Load(file);
	GLuint texture;
	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	SDL_PixelFormat *format = surface->format;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if (format->Amask) {
		debug("RGBA");
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4,
				surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	}
	else {
		debug("RGB");
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
				surface->w, surface->h, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	}
	SDL_FreeSurface(surface);
	return texture;
}

int check_collision(SDL_Rect A, SDL_Rect B)
{

	return !(((A.x >= B.x && A.x < B.x + B.w)  ||
			(B.x >= A.x && B.x < A.x + A.w)) && 
		((A.y >= B.y && A.y < B.y + B.h)   ||
		 (B.y >= A.y && B.y < A.y + A.h)));
}
