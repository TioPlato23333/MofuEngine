/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef UTIL_SHADER_PROGRAM_H_
#define UTIL_SHADER_PROGRAM_H_

#include <GL/glew.h>
#include <memory>

namespace mofu {

class ShaderProgram {
public:
  ShaderProgram();
  virtual ~ShaderProgram();

  bool CreateProgram(const char *vertex_shader, const char *fragment_shader);

protected:
  GLint shader_program_id_;
};

class DrawEntityShaderProgram : public ShaderProgram {
public:
  DrawEntityShaderProgram();
  ~DrawEntityShaderProgram() override;

  void Run(GLint tex, GLfloat *vertex_coord, GLfloat *tex_coord);

private:
  GLint location_pos_coord_;
  GLint location_tex_coord_;
  GLint location_input_tex_;
};

class ShaderProgramPool {
public:
  ShaderProgramPool();
  ~ShaderProgramPool();

  void Clear();
  DrawEntityShaderProgram *GetDrawEntityShaderProgram();

private:
  std::unique_ptr<DrawEntityShaderProgram> draw_entity_program_;
};

using ShaderProgramPoolPtr = std::shared_ptr<ShaderProgramPool>;

} // namespace mofu

#endif // UTIL_SHADER_PROGRAM_H_
