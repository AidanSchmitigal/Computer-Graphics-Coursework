#include "textures.h"

#include <stdio.h>

#include "bmptotexture.h"

unsigned char *ReadTexture3D(char *filename, int *width, int *height, int *depth) {
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    fprintf(stderr, "Cannot find the file '%s'\n", filename);
    return NULL;
  }
  int nums,
      numt, nump;
  fread(&nums, 4, 1, fp);
  fread(&numt, 4, 1, fp);
  fread(&nump, 4, 1, fp);
  fprintf(stderr, "Texture size = % d x % d x % d\n", nums, numt, nump);
  *width = nums;
  *height = numt;
  *depth = nump;
  unsigned char *texture = new unsigned char[4 * nums * numt * nump];
  fread(texture, 4 * nums * numt * nump, 1, fp);
  fclose(fp);
  return texture;
}

bool loadTexture(char *file, GLuint *texture, int *width, int *height) {
  unsigned char *textureData = BmpToTexture(file, width, height);
  if (textureData == NULL) {
    fprintf(stderr, "Cannot open texture '%s'\n", file);
    return false;
  } else {
    fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", file, *width, *height);
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
  }
  return true;
}

bool loadTexture3D(char *file, GLuint *texture, int *width, int *height, int *depth) {
  unsigned char *textureData = ReadTexture3D(file, width, height, depth);
  if (textureData == NULL) {
    fprintf(stderr, "Cannot open texture '%s'\n", file);
    return false;
  } else {
    fprintf(stderr, "Opened '%s': width = %d ; height = %d ; depth = %d\n", file, *width, *height, *depth);
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_3D, *texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, *width, *height, *depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
  }
  return true;
}