/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "util/shader_program.h"

#include <cassert>

#include "core/log.h"
#include "util/util.h"

namespace {

GLsizei kMaxErrorLogSize = 256;

constexpr GLfloat kDefaultVertexCoord[][3] = {{-1.0f, -1.0f, 0.0f},
                                              {1.0f, -1.0f, 0.0f},
                                              {-1.0f, 1.0f, 0.0f},
                                              {1.0f, 1.0f, 0.0f}};

constexpr GLfloat kDefaultTexCoord[][2] = {
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 0.0f},
};

constexpr char kGlvsDrawEntity[] =
    " #version 120                                            \n"
    " attribute vec3 posCoord;                                \n"
    " attribute vec2 texCoord;                                \n"
    " varying vec2 texCoord0;                                 \n"
    " void main()                                             \n"
    " {                                                       \n"
    "   gl_Position = vec4(posCoord.xyz, 1.0);                \n"
    "   texCoord0 = texCoord.xy;                              \n"
    " }                                                       \n";

constexpr char kGlfsDrawEntity[] =
    " #version 120                                            \n"
    " uniform sampler2D inputTex;                             \n"
    " varying vec2 texCoord0;                                 \n"
    " void main()                                             \n"
    " {                                                       \n"
    "   gl_FragColor = texture2D(inputTex, texCoord0);        \n"
    " }                                                       \n";

} // namespace

namespace mofu {

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
    : location_pos_coord_(0), location_tex_coord_(0), location_input_tex_(0) {}

DrawEntityShaderProgram::~DrawEntityShaderProgram() {
  if (shader_program_id_) {
    glDeleteProgram(shader_program_id_);
  }
}

void DrawEntityShaderProgram::Run(GLint tex, GLfloat *vertex_coord,
                                  GLfloat *tex_coord) {
  glUseProgram(shader_program_id_);
  CheckGlError("glUseProgram");

  location_input_tex_ = glGetUniformLocation(shader_program_id_, "inputTex");
  glUniform1i(location_input_tex_, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
  CheckGlError("glBindTexture");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  location_pos_coord_ = glGetAttribLocation(shader_program_id_, "posCoord");
  glVertexAttribPointer(location_pos_coord_, 3, GL_FLOAT, GL_FALSE, 0,
                        vertex_coord ? vertex_coord
                                     : (GLfloat *)kDefaultVertexCoord);
  glEnableVertexAttribArray(location_pos_coord_);
  CheckGlError("glEnableVertexAttribArray");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  location_tex_coord_ = glGetAttribLocation(shader_program_id_, "texCoord");
  glVertexAttribPointer(location_tex_coord_, 2, GL_FLOAT, GL_FALSE, 0,
                        tex_coord ? tex_coord : (GLfloat *)kDefaultTexCoord);
  glEnableVertexAttribArray(location_tex_coord_);
  CheckGlError("glEnableVertexAttribArray");

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  CheckGlError("glDrawArrays");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}

ShaderProgramPool::ShaderProgramPool() : draw_entity_program_(nullptr) {}

ShaderProgramPool::~ShaderProgramPool() { Clear(); }

void ShaderProgramPool::Clear() { draw_entity_program_.reset(); }

DrawEntityShaderProgram *ShaderProgramPool::GetDrawEntityShaderProgram() {
  if (!draw_entity_program_) {
    draw_entity_program_.reset(new DrawEntityShaderProgram());
    draw_entity_program_->CreateProgram(kGlvsDrawEntity, kGlfsDrawEntity);
  }
  return draw_entity_program_.get();
}

} // namespace mofu
