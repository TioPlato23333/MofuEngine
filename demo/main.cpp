/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "template/tetris/tetris.h"

int main(int argc, char *argv[]) {
  mofu::TetrisPtr tetris = std::make_shared<mofu::Tetris>();
  tetris->InitGame();
  tetris->RunGame();
  return 0;
}
