#pragma once

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

#include <string>
#include <variant>
#include <vector>

#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "terrain.h"
#include "utils/glslprogram.h"

namespace ShaderEditor {

struct Uniform {
  GLint size;
  GLenum type;
  std::string name;
  bool missing = false;
  std::variant<float, glm::vec2, glm::vec3, glm::vec4, double, int, glm::ivec2, glm::ivec3, glm::ivec4, uint, bool, glm::bvec2, glm::bvec3, glm::bvec4, glm::mat2, glm::mat3, glm::mat4, glm::mat2x3, glm::mat2x4, glm::mat3x2, glm::mat3x4, glm::mat4x2, glm::mat4x3> value;
  // GL_FLOAT:                "float";           float
  // GL_FLOAT_VEC2:           "vec2";            glm::vec2
  // GL_FLOAT_VEC3:           "vec3";            glm::vec3
  // GL_FLOAT_VEC4:           "vec4";            glm::vec4
  // GL_DOUBLE:               "double";          double
  // GL_INT:                  "int";             int
  // GL_INT_VEC2:             "ivec2";           glm::ivec2
  // GL_INT_VEC3:             "ivec3";           glm::ivec3
  // GL_INT_VEC4:             "ivec4";           glm::ivec4
  // GL_UNSIGNED_INT:         "unsigned int";    uint
  // GL_BOOL:                 "bool";            bool
  // GL_BOOL_VEC2:            "bvec2";           glm::bvec2
  // GL_BOOL_VEC3:            "bvec3";           glm::bvec3
  // GL_BOOL_VEC4:            "bvec4";           glm::bvec4
  // GL_FLOAT_MAT2:           "mat2";            glm::mat2
  // GL_FLOAT_MAT3:           "mat3";            glm::mat3
  // GL_FLOAT_MAT4:           "mat4";            glm::mat4
  // GL_FLOAT_MAT2x3:         "mat2x3";          glm::mat2x3
  // GL_FLOAT_MAT2x4:         "mat2x4";          glm::mat2x4
  // GL_FLOAT_MAT3x2:         "mat3x2";          glm::mat3x2
  // GL_FLOAT_MAT3x4:         "mat3x4";          glm::mat3x4
  // GL_FLOAT_MAT4x2:         "mat4x2";          glm::mat4x2
  // GL_FLOAT_MAT4x3:         "mat4x3";          glm::mat4x3
  // GL_SAMPLER_1D:           "sampler1D";       GLuint
  // GL_SAMPLER_2D:           "sampler2D";       GLuint
  // GL_SAMPLER_3D:           "sampler3D";       GLuint
  // GL_SAMPLER_CUBE:         "samplerCube";     GLuint
  // GL_SAMPLER_1D_SHADOW:    "sampler1DShadow"; GLuint
  // GL_SAMPLER_2D_SHADOW:    "sampler2DShadow"; GLuint

  Uniform(GLint size, GLenum type, char* name) : size(size), type(type), name(name), missing(false) {}
  Uniform(GLint size, GLenum type, char* name, bool missing) : size(size), type(type), name(name), missing(missing) {}
};

std::string GetGLTypeString(GLenum type);
void InitializeUniform(Uniform& uniform);
char* SerializeUniform(const Uniform& uniform);
void SetShaderUniform(Uniform& uniform, GLSLProgram& program);
}  // namespace ShaderEditor