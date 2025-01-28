#pragma once

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

unsigned char *ReadTexture3D(char *filename, int *width, int *height, int *depth);
bool loadTexture(char *file, GLuint *texture, int *width, int *height);
bool loadTexture3D(char *file, GLuint textureSlot, GLuint *texture, int *width, int *height, int *depth);