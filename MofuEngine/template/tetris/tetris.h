/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef TEMPLATE_TETRIS_TETRIS_H_
#define TEMPLATE_TETRIS_TETRIS_H_

#include "util/renderer.h"

namespace mofu {

class TetrisRule : public Rule {
public:
  enum SquareType : int16_t {
    kEmpty = 0,
    kDead,
    kType0,
    kTypeMax,
  };

  struct Square {
    int x;
    int y;
    std::vector<std::vector<SquareType>> shape;
  };

public:
  TetrisRule(int row_number, int col_number, float square_width,
             float square_height);
  ~TetrisRule() override = default;

  void ChangeWorld(WorldPtr world)
      override; // must know the objects in the world before changing it
  void InitChessBoard();
  void PlaceActiveSquare(const Square &square); // square coordinate is the (x,
                                                // y) of left upper point
  void SqaureDown();
  void SqaureLeft();
  void SqaureRight();
  void SqaureRotate();

private:
  void ResetChessBoard(); // draw the chess board according to original board
                          // and active square
  bool CheckTouchBottom();
  void CheckGameOver();
  void MakeActiveSquareDead();
  int CheckDeadSqaureWiped();

  std::vector<std::vector<SquareType>> chess_board_;
  int row_number_;
  int col_number_;
  float square_width_;
  float square_height_;
  Square active_square_;
  bool is_active_square_exist_;
  int score_;
  bool is_game_over_;
};

using TetrisRulePtr = std::shared_ptr<TetrisRule>;

TetrisRule::Square GenerateRandomInitSquare();

class Tetris {
public:
  Tetris();
  ~Tetris() = default;

  void InitGame();
  void RunGame();

private:
  RendererPtr renderer_;
  TetrisRulePtr rule_;
};

using TetrisPtr = std::shared_ptr<Tetris>;

} // namespace mofu

#endif // TEMPLATE_TETRIS_TETRIS_H_
