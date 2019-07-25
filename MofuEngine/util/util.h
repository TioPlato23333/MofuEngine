/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef UTIL_UTIL_H_
#define UTIL_UTIL_H_

#include <GL/glew.h>

#include "core/core.h"

namespace mofu {

GLenum CheckGlError(const char *message);
GLuint CreateTexFromPixelData(uint8_t *data, int width, int height);
void AddRenderVetexFromRect2D(const VideoEntityPtr &object, float x, float y,
                              float width, float height, float window_width,
                              float window_height);
void SetRenderVetexFromRect2D(const VideoEntityPtr &object, float x, float y,
                              float width, float height, float window_width,
                              float window_height);

} // namespace mofu

#endif // UTIL_UTIL_H_
