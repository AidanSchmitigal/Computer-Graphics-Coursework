#include "uniform.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "scene.h"
#include "shader.h"
#include "utils/texture.h"

namespace ShaderEditor {

// MARK: GetGLTypeString
std::string GetGLTypeString(GLenum type) {
  switch (type) {
    case GL_FLOAT:
      return "float";
    case GL_FLOAT_VEC2:
      return "vec2";
    case GL_FLOAT_VEC3:
      return "vec3";
    case GL_FLOAT_VEC4:
      return "vec4";
    case GL_DOUBLE:
      return "double";
    case GL_INT:
      return "int";
    case GL_INT_VEC2:
      return "ivec2";
    case GL_INT_VEC3:
      return "ivec3";
    case GL_INT_VEC4:
      return "ivec4";
    case GL_UNSIGNED_INT:
      return "unsigned int";
    case GL_BOOL:
      return "bool";
    case GL_BOOL_VEC2:
      return "bvec2";
    case GL_BOOL_VEC3:
      return "bvec3";
    case GL_BOOL_VEC4:
      return "bvec4";
    case GL_FLOAT_MAT2:
      return "mat2";
    case GL_FLOAT_MAT3:
      return "mat3";
    case GL_FLOAT_MAT4:
      return "mat4";
    case GL_FLOAT_MAT2x3:
      return "mat2x3";
    case GL_FLOAT_MAT2x4:
      return "mat2x4";
    case GL_FLOAT_MAT3x2:
      return "mat3x2";
    case GL_FLOAT_MAT3x4:
      return "mat3x4";
    case GL_FLOAT_MAT4x2:
      return "mat4x2";
    case GL_FLOAT_MAT4x3:
      return "mat4x3";
    case GL_SAMPLER_1D:
      return "sampler1D";
    case GL_SAMPLER_2D:
      return "sampler2D";
    case GL_SAMPLER_3D:
      return "sampler3D";
    case GL_SAMPLER_CUBE:
      return "samplerCube";
    case GL_SAMPLER_1D_SHADOW:
      return "sampler1DShadow";
    case GL_SAMPLER_2D_SHADOW:
      return "sampler2DShadow";
    default:
      return "unknown";
  }
}

// MARK: InitializeUniform
void InitializeUniform(Uniform& uniform) {
  switch (uniform.type) {
    case GL_FLOAT:
      if (uniform.name == "uKa")
        uniform.value = (float)0.1f;
      else if (uniform.name == "uKd")
        uniform.value = (float)0.9f;
      else if (uniform.name == "uKs")
        uniform.value = (float)0.9f;
      else if (uniform.name == "uShininess")
        uniform.value = (float)10.0f;
      else
        uniform.value = (float)0.0f;
      break;
    case GL_FLOAT_VEC2:
      uniform.value = glm::vec2();
      break;
    case GL_FLOAT_VEC3:
      if (uniform.name.find("Color") != std::string::npos)
        uniform.value = glm::vec3(static_cast<float>(rand()), static_cast<float>(rand()), static_cast<float>(rand())) / static_cast<float>(RAND_MAX);
      else
        uniform.value = glm::vec3();

      break;
    case GL_FLOAT_VEC4:
      uniform.value = glm::vec4();
      break;
    case GL_DOUBLE:
      uniform.value = (double)0.0;
      break;
    case GL_INT:
      uniform.value = (int)0;
      break;
    case GL_INT_VEC2:
      uniform.value = glm::ivec2();
      break;
    case GL_INT_VEC3:
      uniform.value = glm::ivec3();
      break;
    case GL_INT_VEC4:
      uniform.value = glm::ivec4();
      break;
    case GL_UNSIGNED_INT:
      uniform.value = (uint)0;
      break;
    case GL_BOOL:
      uniform.value = (bool)false;
      break;
    case GL_BOOL_VEC2:
      uniform.value = glm::bvec2();
      break;
    case GL_BOOL_VEC3:
      uniform.value = glm::bvec3();
      break;
    case GL_BOOL_VEC4:
      uniform.value = glm::bvec4();
      break;
    case GL_FLOAT_MAT2:
      uniform.value = glm::mat2();
      break;
    case GL_FLOAT_MAT3:
      uniform.value = glm::mat3();
      break;
    case GL_FLOAT_MAT4:
      uniform.value = glm::mat4();
      break;
    case GL_FLOAT_MAT2x3:
      uniform.value = glm::mat2x3();
      break;
    case GL_FLOAT_MAT2x4:
      uniform.value = glm::mat2x4();
      break;
    case GL_FLOAT_MAT3x2:
      uniform.value = glm::mat3x2();
      break;
    case GL_FLOAT_MAT3x4:
      uniform.value = glm::mat3x4();
      break;
    case GL_FLOAT_MAT4x2:
      uniform.value = glm::mat4x2();
      break;
    case GL_FLOAT_MAT4x3:
      uniform.value = glm::mat4x3();
      break;
    case GL_SAMPLER_1D:
      break;
    case GL_SAMPLER_2D:
      uniform.value = (uint)0;
      break;
    case GL_SAMPLER_3D:
      uniform.value = (uint)0;
      break;
    case GL_SAMPLER_CUBE:
      break;
    case GL_SAMPLER_1D_SHADOW:
      break;
    case GL_SAMPLER_2D_SHADOW:
      break;
    default:
      printf("Unknown type: %d\n", uniform.type);
      uniform.value = 0;  // Default fallback
      break;
  }
}

// MARK: Serialize Uniform
char* SerializeUniform(const Uniform& uniform) {
  char* str = new char[1024];
  switch (uniform.type) {
    case GL_FLOAT: {
      auto& v = std::get<float>(uniform.value);
      sprintf(str, "GL_FLOAT;%f", v);
    } break;
    case GL_FLOAT_VEC2: {
      auto& v = std::get<glm::vec2>(uniform.value);
      sprintf(str, "GL_FLOAT_VEC2;%f;%f", v[0], v[1]);
    } break;
    case GL_FLOAT_VEC3: {
      auto& v = std::get<glm::vec3>(uniform.value);
      sprintf(str, "GL_FLOAT_VEC3;%f;%f;%f", v[0], v[1], v[2]);
    } break;
    case GL_FLOAT_VEC4: {
      auto& v = std::get<glm::vec4>(uniform.value);
      sprintf(str, "GL_FLOAT_VEC4;%f;%f;%f;%f", v[0], v[1], v[2], v[3]);
    } break;
    case GL_DOUBLE: {
      auto& v = std::get<double>(uniform.value);
      sprintf(str, "GL_DOUBLE;%f", v);
    } break;
    case GL_INT: {
      auto& v = std::get<int>(uniform.value);
      sprintf(str, "GL_INT;%d", v);
    } break;
    case GL_INT_VEC2: {
      auto& v = std::get<glm::ivec2>(uniform.value);
      sprintf(str, "GL_INT_VEC2;%d;%d", v[0], v[1]);
    } break;
    case GL_INT_VEC3: {
      auto& v = std::get<glm::ivec3>(uniform.value);
      sprintf(str, "GL_INT_VEC3;%d;%d;%d", v[0], v[1], v[2]);
    } break;
    case GL_INT_VEC4: {
      auto& v = std::get<glm::ivec4>(uniform.value);
      sprintf(str, "GL_INT_VEC4;%d;%d;%d;%d", v[0], v[1], v[2], v[3]);
    } break;
    case GL_UNSIGNED_INT: {
      auto& v = std::get<uint>(uniform.value);
      sprintf(str, "GL_UNSIGNED_INT;%u", v);
    } break;
    case GL_BOOL: {
      auto& v = std::get<bool>(uniform.value);
      sprintf(str, "GL_BOOL;%d", v);
    } break;
    case GL_BOOL_VEC2: {
      auto& v = std::get<glm::bvec2>(uniform.value);
      sprintf(str, "GL_BOOL_VEC2;%d;%d", v[0], v[1]);
    } break;
    case GL_BOOL_VEC3: {
      auto& v = std::get<glm::bvec3>(uniform.value);
      sprintf(str, "GL_BOOL_VEC3;%d;%d;%d", v[0], v[1], v[2]);
    } break;
    case GL_BOOL_VEC4: {
      auto& v = std::get<glm::bvec4>(uniform.value);
      sprintf(str, "GL_BOOL_VEC4;%d;%d;%d;%d", v[0], v[1], v[2], v[3]);
    } break;
    case GL_FLOAT_MAT2: {
      auto& v = std::get<glm::mat2>(uniform.value);
      sprintf(str, "GL_FLOAT_MAT2;%f;%f;%f;%f", v[0][0], v[0][1], v[1][0], v[1][1]);
    } break;
    case GL_FLOAT_MAT3: {
      auto& v = std::get<glm::mat3>(uniform.value);
      sprintf(str, "GL_FLOAT_MAT3;%f;%f;%f;%f;%f;%f;%f;%f;%f", v[0][0], v[0][1], v[0][2], v[1][0], v[1][1], v[1][2], v[2][0], v[2][1], v[2][2]);
    } break;
    case GL_FLOAT_MAT4: {
      auto& v = std::get<glm::mat4>(uniform.value);
      sprintf(str, "GL_FLOAT_MAT4;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f", v[0][0], v[0][1], v[0][2], v[0][3], v[1][0], v[1][1], v[1][2], v[1][3], v[2][0], v[2][1], v[2][2], v[2][3], v[3][0], v[3][1], v[3][2], v[3][3]);
    } break;
    case GL_FLOAT_MAT2x3: {
      auto& v = std::get<glm::mat2x3>(uniform.value);
      sprintf(str, "GL_FLOAT_MAT2x3;%f;%f;%f;%f;%f;%f", v[0][0], v[0][1], v[0][2], v[1][0], v[1][1], v[1][2]);
    } break;
    case GL_FLOAT_MAT2x4: {
      auto& v = std::get<glm::mat2x4>(uniform.value);
      sprintf(str, "GL_FLOAT_MAT2x4;%f;%f;%f;%f;%f;%f;%f;%f", v[0][0], v[0][1], v[0][2], v[0][3], v[1][0], v[1][1], v[1][2], v[1][3]);
    } break;
    case GL_FLOAT_MAT3x2: {
      auto& v = std::get<glm::mat3x2>(uniform.value);
      sprintf(str, "GL_FLOAT_MAT3x2;%f;%f;%f;%f;%f;%f", v[0][0], v[0][1], v[0][2], v[1][0], v[1][1], v[1][2]);
    } break;
    case GL_FLOAT_MAT3x4: {
      auto& v = std::get<glm::mat3x4>(uniform.value);
      sprintf(str, "GL_FLOAT_MAT3x4;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f", v[0][0], v[0][1], v[0][2], v[0][3], v[1][0], v[1][1], v[1][2], v[1][3], v[2][0], v[2][1], v[2][2], v[2][3], v[3][0], v[3][1], v[3][2], v[3][3]);
    } break;
    case GL_FLOAT_MAT4x2: {
      auto& v = std::get<glm::mat4x2>(uniform.value);
      sprintf(str, "GL_FLOAT_MAT4x2;%f;%f;%f;%f;%f;%f;%f;%f", v[0][0], v[0][1], v[1][0], v[1][1], v[2][0], v[2][1], v[3][0], v[3][1]);
    } break;
    case GL_FLOAT_MAT4x3: {
      auto& v = std::get<glm::mat4x3>(uniform.value);
      sprintf(str, "GL_FLOAT_MAT4x3;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f", v[0][0], v[0][1], v[0][2], v[0][3], v[1][0], v[1][1], v[1][2], v[1][3], v[2][0], v[2][1], v[2][2], v[2][3], v[3][0], v[3][1], v[3][2], v[3][3]);
    } break;
    case GL_SAMPLER_1D: {
      auto& v = std::get<uint>(uniform.value);
      sprintf(str, "GL_SAMPLER_1D;%u", v);
    } break;
    case GL_SAMPLER_2D: {
      auto& v = std::get<uint>(uniform.value);
      sprintf(str, "GL_SAMPLER_2D;%u", v);
    } break;
    case GL_SAMPLER_3D: {
      auto& v = std::get<uint>(uniform.value);
      sprintf(str, "GL_SAMPLER_3D;%u", v);
    } break;
    case GL_SAMPLER_CUBE: {
      auto& v = std::get<uint>(uniform.value);
      sprintf(str, "GL_SAMPLER_CUBE;%u", v);
    } break;
    case GL_SAMPLER_1D_SHADOW: {
      auto& v = std::get<uint>(uniform.value);
      sprintf(str, "GL_SAMPLER_1D_SHADOW;%u", v);
    } break;
    case GL_SAMPLER_2D_SHADOW: {
      auto& v = std::get<uint>(uniform.value);
      sprintf(str, "GL_SAMPLER_2D_SHADOW;%u", v);
    } break;
    default:
      sprintf(str, "unknown;%u", uniform.type);
      break;
  }
  return str;
}

// MARK: SetShaderUniform
void SetShaderUniform(Uniform& uniform, GLSLProgram& program) {
  if (uniform.type == GL_FLOAT) {
    auto& v = std::get<float>(uniform.value);

    if (uniform.name == "uTime")
      v = Scene::Time;
    else if (uniform.name == "uTimeDelta")
      v = Scene::DeltaTime;
    else if (uniform.name == "uTimeLoop")
      v = fmodf(Scene::Time, 1.f);
    else if (uniform.name == "uTimeLoop10")
      v = fmodf(Scene::Time, 10.f);

    program.SetUniformVariable((char*)uniform.name.c_str(), v);
  } else if (uniform.type == GL_FLOAT_VEC2) {
    auto& v = std::get<glm::vec2>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_VEC3) {
    auto& v = std::get<glm::vec3>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_VEC4) {
    auto& v = std::get<glm::vec4>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_DOUBLE) {
    auto& v = std::get<double>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), v);
  } else if (uniform.type == GL_INT) {
    auto& v = std::get<int>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), v);
  } else if (uniform.type == GL_INT_VEC2) {
    auto& v = std::get<glm::ivec2>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_INT_VEC3) {
    auto& v = std::get<glm::ivec3>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_INT_VEC4) {
    auto& v = std::get<glm::ivec4>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_UNSIGNED_INT) {
    auto& v = std::get<uint>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), v);
  } else if (uniform.type == GL_BOOL) {
    auto& v = std::get<bool>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), v);
  } else if (uniform.type == GL_BOOL_VEC2) {
    auto& v = std::get<glm::bvec2>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_BOOL_VEC3) {
    auto& v = std::get<glm::bvec3>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_BOOL_VEC4) {
    auto& v = std::get<glm::bvec4>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT2) {
    auto& v = std::get<glm::mat2>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT3) {
    auto& v = std::get<glm::mat3>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT4) {
    auto& v = std::get<glm::mat4>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT2x3) {
    auto& v = std::get<glm::mat2x3>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT2x4) {
    auto& v = std::get<glm::mat2x4>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT3x2) {
    auto& v = std::get<glm::mat3x2>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT3x4) {
    auto& v = std::get<glm::mat3x4>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT4x2) {
    auto& v = std::get<glm::mat4x2>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT4x3) {
    auto& v = std::get<glm::mat4x3>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT2x3) {
    auto& v = std::get<glm::mat2x3>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT2x4) {
    auto& v = std::get<glm::mat2x4>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT3x4) {
    auto& v = std::get<glm::mat3x4>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT4x2) {
    auto& v = std::get<glm::mat4x2>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT4x3) {
    auto& v = std::get<glm::mat4x3>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT3x2) {
    auto& v = std::get<glm::mat3x2>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT3x4) {
    auto& v = std::get<glm::mat3x4>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_FLOAT_MAT4x2) {
    auto& v = std::get<glm::mat4x2>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
  } else if (uniform.type == GL_SAMPLER_1D) {
    auto& v = std::get<uint>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), v);
  } else if (uniform.type == GL_SAMPLER_2D) {
    auto& v = std::get<uint>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), 1);  // TODO: Check if this number is correct
    // program.SetUniformVariable((char*)uniform.name.c_str(), Textures::noise2dTexture);  // TODO: Check if this number is correct
  } else if (uniform.type == GL_SAMPLER_3D) {
    auto& v = std::get<uint>(uniform.value);
    program.SetUniformVariable((char*)uniform.name.c_str(), 0);
    // program.SetUniformVariable((char*)uniform.name.c_str(), Textures::noise3dTexture);
  } else if (uniform.type == GL_SAMPLER_CUBE) {
    // auto& v = std::get<GLuint>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), v);
  } else if (uniform.type == GL_SAMPLER_1D_SHADOW) {
    // auto& v = std::get<GLuint>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), v);
  } else if (uniform.type == GL_SAMPLER_2D_SHADOW) {
    // auto& v = std::get<GLuint>(uniform.value);
    // program.SetUniformVariable((char*)uniform.name.c_str(), v);
  }
}
}  // namespace ShaderEditor