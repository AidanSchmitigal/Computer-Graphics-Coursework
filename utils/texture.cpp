#include "texture.h"

#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "bmptotexture.h"
#include "utils/stb_image.h"

namespace Textures {
GLuint noise2dTexture;
GLuint noise3dTexture;
GLuint nvidiaCubeTexture;

// MARK: texture_setup
void InitTextures() {
  printf("Initializing noise textures\n");
  const char *noise2d_path = "textures/noise2d.064.raw";
  const char *noise3d_path = "textures/noise3d.064.raw";
  const char *nvidia_cube_path[6] = {"textures/nvposx.bmp", "textures/nvnegx.bmp", "textures/nvposy.bmp", "textures/nvnegy.bmp", "textures/nvposz.bmp", "textures/nvnegz.bmp"};

  bool loaded = loadTexture2D(noise2d_path, GL_TEXTURE0, &Textures::noise2dTexture);

  if (!loaded) {
    fprintf(stderr, "Could not load 2D noise texture from file\n");
    return;
  }
  printf("Created 2D noise texture id %d, slot %d, %d\n", Textures::noise2dTexture, GL_TEXTURE0, GL_TEXTURE0 - GL_TEXTURE0);

  loaded = loadTexture3D(noise3d_path, GL_TEXTURE1, &Textures::noise3dTexture);

  if (!loaded) {
    fprintf(stderr, "Could not load 3D noise texture from file\n");
    return;
  }
  printf("Created 3D noise texture id %d, slot %d, %d\n", Textures::noise3dTexture, GL_TEXTURE1, GL_TEXTURE1 - GL_TEXTURE0);

  loaded = loadTextureCube(nvidia_cube_path, GL_TEXTURE2, &Textures::nvidiaCubeTexture);

  if (!loaded) {
    fprintf(stderr, "Could not load nvidia cube texture from file\n");
    return;
  }

  printf("Created nvidia cube texture id %d, slot %d, %d\n", Textures::nvidiaCubeTexture, GL_TEXTURE2, GL_TEXTURE2 - GL_TEXTURE0);

  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    printf("Errors after loading textures: %s\n", gluErrorString(err));
  }
}
}  // namespace Textures

GLFWimage loadImage(const char *filename) {
  GLFWimage image;
  int channels;

  image.pixels = stbi_load(filename, &image.width, &image.height, &channels, 4);

  if (!image.pixels) {
    fprintf(stderr, "Failed to load image '%s'\n", filename);
    return image;
  }

  return image;
}

unsigned char *ReadTexture3D(const char *filename, int *width, int *height, int *depth) {
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

unsigned char *ReadTexture2D(const char *filename, int *width, int *height) {
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
    return NULL;
  int nums, numt;
  fread(&nums, 4, 1, fp);
  fread(&numt, 4, 1, fp);
  fprintf(stderr, "Texture size = % d x % d\n", nums, numt);
  *width = nums;
  *height = numt;
  unsigned char *texture = new unsigned char[4 * nums * numt];
  fread(texture, 4 * nums * numt, 1, fp);
  fclose(fp);
  return texture;
}

bool loadImageTexture(const char *file, GLuint *texture, int *width, int *height) {
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

bool loadTexture2D(const char *file, GLuint textureSlot, GLuint *texture) {
  int width, height;
  unsigned char *textureData = ReadTexture2D(file, &width, &height);
  if (textureData == NULL) {
    fprintf(stderr, "Cannot open texture '%s'\n", file);
    return false;
  } else {
    fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", file, width, height);
    glGenTextures(1, texture);
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glActiveTexture(textureSlot);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

    glBindTexture(GL_TEXTURE_3D, 0);
  }
  return true;
}

bool loadTexture3D(const char *file, GLuint textureSlot, GLuint *texture) {
  int width, height, depth;
  unsigned char *textureData = ReadTexture3D(file, &width, &height, &depth);
  if (textureData == NULL) {
    fprintf(stderr, "Cannot open texture '%s'\n", file);
    return false;
  } else {
    fprintf(stderr, "Opened '%s': width = %d ; height = %d ; depth = %d\n", file, width, height, depth);
    glGenTextures(1, texture);
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_3D, *texture);
    glActiveTexture(textureSlot);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      printf("GL Error after texture creation: %d\n", err);
      return false;
    }

    glBindTexture(GL_TEXTURE_3D, 0);
  }
  return true;
}

bool loadTextureCube(const char *paths[], GLuint textureSlot, GLuint *texture) {
  int size = 512;

  glGenTextures(1, texture);
  glActiveTexture(textureSlot);
  glBindTexture(GL_TEXTURE_CUBE_MAP, *texture);
  glActiveTexture(textureSlot);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  for (int file = 0; file < 6; file++) {
    int nums, numt;
    unsigned char *texture2d = BmpToTexture(paths[file], &nums, &numt);
    if (texture2d == NULL)
      fprintf(stderr, "Could not open BMP 2D texture '%s'", paths[file]);
    else
      fprintf(stderr, "BMP 2D texture '%s' read -- nums = %d, numt = %d\n", paths[file], nums, numt);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + file, 0, 3, nums, numt, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, texture2d);
    delete[] texture2d;
  }

  glBindTexture(GL_TEXTURE_3D, 0);

  return true;
}