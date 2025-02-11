#pragma once

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif
#include <GLFW/glfw3.h>

GLFWimage loadImage(const char *filename);
unsigned char *ReadTexture3D(const char *filename, int *width, int *height, int *depth);
bool loadImageTexture(const char *file, GLuint *texture, int *width, int *height);
bool loadTexture2D(const char *file, GLuint textureSlot, GLuint *texture);
bool loadTexture3D(const char *file, GLuint textureSlot, GLuint *texture);
bool loadTextureCube(const char *paths[], GLuint textureSlot, GLuint *texture);

namespace Textures {
extern GLuint noise2dTexture;
extern GLuint noise3dTexture;
extern GLuint nvidiaCubeTexture;

void InitTextures();
}  // namespace Textures