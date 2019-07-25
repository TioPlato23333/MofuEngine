/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "util/util.h"
#include "core/log.h"

namespace mofu {

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

void AddRenderVetexFromRect2D(const VideoEntityPtr &object, float x, float y,
                              float width, float height, float window_width,
                              float window_height) {
  object->AddRenderVertex(std::make_tuple(
      x / window_width * 2.0f - 1.0f, y / window_height * 2.0f - 1.0f, 0.0f));
  object->AddRenderVertex(
      std::make_tuple((x + width) / window_width * 2.0f - 1.0f,
                      y / window_height * 2.0f - 1.0f, 0.0f));
  object->AddRenderVertex(
      std::make_tuple(x / window_width * 2.0f - 1.0f,
                      (y + height) / window_height * 2.0f - 1.0f, 0.0f));
  object->AddRenderVertex(
      std::make_tuple((x + width) / window_width * 2.0f - 1.0f,
                      (y + height) / window_height * 2.0f - 1.0f, 0.0f));
}

void SetRenderVetexFromRect2D(const VideoEntityPtr &object, float x, float y,
                              float width, float height, float window_width,
                              float window_height) {
  std::vector<VideoEntity::Vertex> vertice;
  vertice.emplace_back(std::make_tuple(x / window_width * 2.0f - 1.0f,
                                       y / window_height * 2.0f - 1.0f, 0.0f));
  vertice.emplace_back(std::make_tuple((x + width) / window_width * 2.0f - 1.0f,
                                       y / window_height * 2.0f - 1.0f, 0.0f));
  vertice.emplace_back(
      std::make_tuple(x / window_width * 2.0f - 1.0f,
                      (y + height) / window_height * 2.0f - 1.0f, 0.0f));
  vertice.emplace_back(
      std::make_tuple((x + width) / window_width * 2.0f - 1.0f,
                      (y + height) / window_height * 2.0f - 1.0f, 0.0f));
  object->SetRenderVertice(vertice);
}

} // namespace mofu
