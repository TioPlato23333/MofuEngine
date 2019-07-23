/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef TEMPLATE_TETRIS_TETRIS_H_
#define TEMPLATE_TETRIS_TETRIS_H_

#include "util/renderer.h"

namespace mofu {

class Tetris {
public:
  Tetris();
  ~Tetris() = default;

  void InitGame();
  void RunGame();

private:
  RendererPtr renderer_;
};

using TetrisPtr = std::shared_ptr<Tetris>;

} // namespace mofu

#endif // TEMPLATE_TETRIS_TETRIS_H_
