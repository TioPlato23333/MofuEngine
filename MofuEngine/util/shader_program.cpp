/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "util/shader_program.h"

#include <cassert>

#include "core/log.h"

namespace {

GLsizei kMaxErrorLogSize = 256;

constexpr GLfloat kDefaultCood[][5] = {
    {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
};

constexpr char kGlvsDrawEntity[] =
    " #version 450 core                                       \n"
    " in vec4 posCoord;                                       \n"
    " in vec4 texCoord;                                       \n"
    " out vec2 texCoord0;                                     \n"
    " void main()                                             \n"
    " {                                                       \n"
    "   gl_Position = posCoord;                               \n"
    "   texCoord0 = texCoord.xy;                              \n"
    " }                                                       \n";

constexpr char kGlfsDrawEntity[] =
    " #version 450 core                                       \n"
    " uniform sampler2D inputTex;                             \n"
    " in vec2 texCoord0;                                      \n"
    " out vec4 color;                                         \n"
    " void main()                                             \n"
    " {                                                       \n"
    "   color = texture(inputTex, texCoord0);                 \n"
    " }                                                       \n";

} // namespace

namespace mofu {

namespace gl {

GLenum CheckGlError(const char *message) {
  GLenum error;
  while ((error = glGetError())) {
    LOGE("Get gl error (0x%x) after %s.", error, message);
#if DEBUG
    assert(false);
#endif
  }
  return error;
}

GLuint CreateTexFromPixelData(uint8_t *data, int width, int height) {
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  CheckGlError("glBindTexture");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  CheckGlError("glTexImage2D");

  glBindTexture(GL_TEXTURE_2D, 0);
  return tex;
}

} // namespace gl

ShaderProgram::ShaderProgram() : shader_program_id_(0) {}

ShaderProgram::~ShaderProgram() {
  if (shader_program_id_) {
    glDeleteProgram(shader_program_id_);
  }
}

bool ShaderProgram::CreateProgram(const char *vertex_shader,
                                  const char *fragment_shader) {
  GLint success;
  GLint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader_id, 1, &vertex_shader, nullptr);
  glCompileShader(vertex_shader_id);
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    GLchar message[kMaxErrorLogSize];
    glGetShaderInfoLog(vertex_shader_id, sizeof(message), &kMaxErrorLogSize,
                       message);
    LOGE("Vertex shader compile failed. Error: %s", message);
    return false;
  }

  GLint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_id, 1, &fragment_shader, nullptr);
  glCompileShader(fragment_shader_id);
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    GLchar message[kMaxErrorLogSize];
    glGetShaderInfoLog(vertex_shader_id, sizeof(message), &kMaxErrorLogSize,
                       message);
    LOGE("Fragment shader compile failed. Error: %s", message);
    return false;
  }

  shader_program_id_ = glCreateProgram();
  glAttachShader(shader_program_id_, vertex_shader_id);
  glAttachShader(shader_program_id_, fragment_shader_id);
  glLinkProgram(shader_program_id_);
  glGetProgramiv(shader_program_id_, GL_LINK_STATUS, &success);
  if (success != GL_TRUE) {
    LOGE("Shader program link failed.");
    return false;
  }

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);
  return true;
}

DrawEntityShaderProgram::DrawEntityShaderProgram()
    : vao_(0), vbo_(0), location_pos_coord_(0), location_tex_coord_(0),
      location_input_tex_(0) {
  shader_program_id_ = CreateProgram(kGlvsDrawEntity, kGlfsDrawEntity);
}

DrawEntityShaderProgram::~DrawEntityShaderProgram() {
  if (shader_program_id_) {
    glDeleteProgram(shader_program_id_);
  }
  if (vbo_) {
    glDeleteBuffers(1, &vbo_);
  }
  if (vao_) {
    glDeleteVertexArrays(1, &vao_);
  }
}

void DrawEntityShaderProgram::Run(GLint tex) {
  glUseProgram(shader_program_id_);
  gl::CheckGlError("glUseProgram");

  location_input_tex_ = glGetUniformLocation(shader_program_id_, "inputTex");
  glUniform1i(location_input_tex_, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
  gl::CheckGlError("glBindTexture");

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  gl::CheckGlError("glBindVertexArray");

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kDefaultCood), kDefaultCood,
               GL_STATIC_DRAW);
  gl::CheckGlError("glBufferData");

  location_pos_coord_ = glGetAttribLocation(shader_program_id_, "posCoord");
  glVertexAttribPointer(location_pos_coord_, 3, GL_FLOAT, GL_FALSE,
                        5 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(location_pos_coord_);
  gl::CheckGlError("glEnableVertexAttribArray");

  location_tex_coord_ = glGetAttribLocation(shader_program_id_, "texCoord");
  glVertexAttribPointer(location_tex_coord_, 2, GL_FLOAT, GL_FALSE,
                        5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(location_tex_coord_);
  gl::CheckGlError("glEnableVertexAttribArray");

  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  gl::CheckGlError("glDrawArrays");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}

ShaderProgramPool::ShaderProgramPool() : draw_entity_program_(nullptr) {}

ShaderProgramPool::~ShaderProgramPool() { Clear(); }

void ShaderProgramPool::Clear() { draw_entity_program_.reset(); }

DrawEntityShaderProgram *ShaderProgramPool::GetDrawEntityShaderProgram() {
  if (!draw_entity_program_) {
    draw_entity_program_.reset(new DrawEntityShaderProgram());
  }
  return draw_entity_program_.get();
}

} // namespace mofu
