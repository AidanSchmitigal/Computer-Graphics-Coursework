#pragma once

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

unsigned char *ReadTexture3D(const char *filename, int *width, int *height, int *depth);
bool loadImageTexture(const char *file, GLuint *texture, int *width, int *height);
bool loadTexture2D(const char *file, GLuint textureSlot, GLuint *texture, int *width, int *height);
bool loadTexture3D(const char *file, GLuint textureSlot, GLuint *texture, int *width, int *height, int *depth);