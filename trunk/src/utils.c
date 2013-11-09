#include "utils.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL2/SDL_image.h>

#include <cuttle/debug.h>
#include <cuttle/utils.h>

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

int check_collision(SDL_Rect A, SDL_Rect B)
{

	return !(((A.x >= B.x && A.x < B.x + B.w)  ||
				(B.x >= A.x && B.x < A.x + A.w)) && 
			((A.y >= B.y && A.y < B.y + B.h)   ||
			 (B.y >= A.y && B.y < A.y + A.h)));
}
