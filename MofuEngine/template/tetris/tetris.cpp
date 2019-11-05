/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "template/tetris/tetris.h"
#include "core/algo.h"
#include "core/log.h"
#include "util/util.h"

namespace {

constexpr char kGameName[] = "Tetris";
constexpr float kSdlWindowWidth = 480.0f;
constexpr float kSdlWindowHeight = 640.0f;
constexpr float kSquareRowNumber = 16.0f;
constexpr float kSquareColNumber = 12.0f;

} // namespace

namespace mofu {

TetrisRule::Square TetrisRule::GetRandomActiveSquare() {
  const static Square kSquareI = {
      0,
      0,
      {{kType0, kType0, kType0, kType0}},
  };
  const static Square kSquareJ = {
      0,
      0,
      {{kType0, kEmpty, kEmpty}, {kType0, kType0, kType0}},
  };
  const static Square kSquareL = {
      0,
      0,
      {{kEmpty, kEmpty, kType0}, {kType0, kType0, kType0}},
  };
  const static Square kSquareO = {
      0,
      0,
      {{kType0, kType0}, {kType0, kType0}},
  };
  const static Square kSquareS = {
      0,
      0,
      {{kEmpty, kType0, kType0}, {kType0, kType0, kEmpty}},
  };
  const static Square kSquareT = {
      0,
      0,
      {{kEmpty, kType0, kEmpty}, {kType0, kType0, kType0}},
  };
  const static Square kSquareZ = {
      0,
      0,
      {{kType0, kType0, kEmpty}, {kEmpty, kType0, kType0}},
  };
  const static std::vector<Square> kSquareList = {
      kSquareI, kSquareJ, kSquareL, kSquareO, kSquareS, kSquareT, kSquareZ,
  };
  return kSquareList[GetRandomNumber(0, kSquareList.size() - 1)];
}

TetrisRule::TetrisRule(int row_number, int col_number, float window_width,
                       float window_height)
    : chess_board_{}, overflow_board_{}, row_number_(row_number),
      col_number_(col_number), window_width_(window_width),
      window_height_(window_height), active_square_{},
      is_active_square_exist_(false), score_(0), is_game_over_(false) {
  square_width_ = window_width / col_number;
  square_height_ = window_height / row_number;
}

void TetrisRule::InitChessBoard() {
  chess_board_ = std::vector<std::vector<SquareType>>(
      row_number_, std::vector<SquareType>(col_number_, kEmpty));
}

void TetrisRule::PlaceActiveSquare(const Square &square) {
  if (is_game_over_) {
    return;
  }
  active_square_ = square;
  active_square_.y = -4;
  active_square_.x = GetRandomNumber(0, col_number_ - square.shape[0].size());
  is_active_square_exist_ = true;
}

void TetrisRule::SqaureDown() {
  if (is_game_over_ || !is_active_square_exist_) {
    return;
  }
  active_square_.y = std::min(
      static_cast<int>(chess_board_.size() - active_square_.shape.size()),
      active_square_.y + 1);
  if (CheckTouchBottom()) {
    MakeActiveSquareDead();
    score_ += CheckDeadSqaureWiped();
    CheckGameOver();
    if (is_game_over_) {
      return;
    }
  }
}

void TetrisRule::SqaureLeft() {
  if (is_game_over_ || !is_active_square_exist_) {
    return;
  }
  bool can_move = true;
  for (int i = active_square_.shape.size() - 1; i >= 0; --i) {
    for (int j = 0; j < active_square_.shape[0].size(); ++j) {
      int x = active_square_.x + j;
      int y = active_square_.y + i;
      if (active_square_.x == 0 ||
          (x >= 0 && x < chess_board_[0].size() && y >= 0 &&
           y < chess_board_.size() && chess_board_[y][x - 1] != kEmpty)) {
        can_move = false;
        break;
      }
    }
    if (!can_move) {
      break;
    }
  }
  if (can_move) {
    active_square_.x -= 1;
  }
  if (CheckTouchBottom()) {
    MakeActiveSquareDead();
    score_ += CheckDeadSqaureWiped();
    CheckGameOver();
    if (is_game_over_) {
      return;
    }
  }
}

void TetrisRule::SqaureRight() {
  if (is_game_over_ || !is_active_square_exist_) {
    return;
  }
  bool can_move = true;
  for (int i = active_square_.shape.size() - 1; i >= 0; --i) {
    for (int j = 0; j < active_square_.shape[0].size(); ++j) {
      int x = active_square_.x + j;
      int y = active_square_.y + i;
      if (active_square_.x ==
              chess_board_[0].size() - active_square_.shape[0].size() ||
          (x >= 0 && x < chess_board_[0].size() && y >= 0 &&
           y < chess_board_.size() && chess_board_[y][x + 1] != kEmpty)) {
        can_move = false;
        break;
      }
    }
    if (!can_move) {
      break;
    }
  }
  if (can_move) {
    active_square_.x += 1;
  }
  if (CheckTouchBottom()) {
    MakeActiveSquareDead();
    score_ += CheckDeadSqaureWiped();
    CheckGameOver();
    if (is_game_over_) {
      return;
    }
  }
}

void TetrisRule::SqaureRotate() {}

bool TetrisRule::CheckTouchBottom() {
  // check from the bottom of square
  for (int i = active_square_.shape.size() - 1; i >= 0; --i) {
    for (int j = 0; j < active_square_.shape[0].size(); ++j) {
      if (active_square_.shape[i][j] == kEmpty) {
        continue;
      }
      int x = active_square_.x + j;
      int y = active_square_.y + i;
      if (y == chess_board_.size() - 1 ||
          (x >= 0 && x < chess_board_[0].size() && y >= 0 &&
           y < chess_board_.size() - 1 && chess_board_[y + 1][x] != kEmpty)) {
        return true;
      }
    }
  }
  return false;
}

void TetrisRule::MakeActiveSquareDead() {
  for (int i = active_square_.shape.size() - 1; i >= 0; --i) {
    int y = active_square_.y + i;
    std::vector<SquareType> temp_overflow_board;
    if (y < 0) {
      temp_overflow_board = std::vector<SquareType>(col_number_, kEmpty);
    }
    for (int j = 0; j < active_square_.shape[0].size(); ++j) {
      int x = active_square_.x + j;
      auto type = active_square_.shape[i][j];
      if (x >= 0 && x < chess_board_[0].size() && y >= 0 &&
          y < chess_board_.size() && type != kEmpty) {
        chess_board_[y][x] = kDead;
      } else if (x >= 0 && x < chess_board_[0].size() && y < 0 &&
                 type != kEmpty) {
        if (!temp_overflow_board.empty()) {
          temp_overflow_board[x] = kDead;
        }
      }
    }
    if (!temp_overflow_board.empty()) {
      overflow_board_.push(temp_overflow_board);
    }
  }
  is_active_square_exist_ = false;
}

int TetrisRule::CheckDeadSqaureWiped() {
  int wiped_line_number = 0;
  for (int i = chess_board_.size() - 1; i >= 0; --i) {
    bool line_wiped = true;
    for (int j = 0; j < chess_board_[0].size(); ++j) {
      line_wiped = line_wiped && (chess_board_[i][j] == kDead);
      if (!line_wiped) {
        break;
      }
    }
    if (line_wiped) {
      for (int j = i; j >= 1; --j) {
        chess_board_[j] = chess_board_[j - 1];
      }
      if (!overflow_board_.empty()) {
        chess_board_[0] = overflow_board_.front();
        overflow_board_.pop();
      } else {
        chess_board_[0] = std::vector<SquareType>(col_number_, kEmpty);
      }
      ++wiped_line_number;
      ++i;
    }
  }
  return wiped_line_number;
}

void TetrisRule::CheckGameOver() {
  is_game_over_ = !overflow_board_.empty();
  if (is_game_over_) {
    LOGI("Game over.");
  }
  overflow_board_ = {};
}

Tetris::Tetris() : renderer_(nullptr), rule_(nullptr) {}

void Tetris::InitGame() {
  rule_ = std::make_shared<TetrisRule>(kSquareRowNumber, kSquareColNumber,
                                       kSdlWindowWidth, kSdlWindowHeight);
  rule_->InitChessBoard();
  renderer_ =
      std::make_shared<Renderer>(kGameName, kSdlWindowWidth, kSdlWindowHeight);
  // action settings
  QuitActionPtr quit_action = std::make_shared<QuitAction>("quit_action");
  quit_action->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE;
  });
  ControlActionPtr move_square_action_up =
      std::make_shared<ControlAction>("up_action");
  move_square_action_up->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP;
  });
  move_square_action_up->SetActionCallback([=](WorldPtr world) {
    rule_->SqaureRotate();
    rule_->ChangeWorld(world);
  });
  ControlActionPtr move_square_action_down =
      std::make_shared<ControlAction>("down_action");
  move_square_action_down->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN;
  });
  move_square_action_down->SetActionCallback([=](WorldPtr world) {
    rule_->SqaureDown();
    rule_->ChangeWorld(world);
  });
  ControlActionPtr move_square_action_left =
      std::make_shared<ControlAction>("left_action");
  move_square_action_left->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT;
  });
  move_square_action_left->SetActionCallback([=](WorldPtr world) {
    rule_->SqaureLeft();
    rule_->ChangeWorld(world);
  });
  ControlActionPtr move_square_action_right =
      std::make_shared<ControlAction>("right_action");
  move_square_action_right->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT;
  });
  move_square_action_right->SetActionCallback([=](WorldPtr world) {
    rule_->SqaureRight();
    rule_->ChangeWorld(world);
  });
  GlobalTimerActionPtr square_fall_action =
      std::make_shared<GlobalTimerAction>("fall_action");
  TimerPtr square_fall_action_timer = std::make_shared<Timer>();
  square_fall_action_timer->SetTimerCallback(
      [=](std::chrono::time_point<std::chrono::system_clock> now) -> bool {
        bool ret = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now - square_fall_action_timer->GetStart())
                       .count() > 500;
        if (ret) {
          square_fall_action_timer->Start();
        }
        return ret;
      });
  square_fall_action->SetTimer(square_fall_action_timer);
  square_fall_action->SetActionCallback([=](WorldPtr world) {
    rule_->SqaureDown();
    rule_->ChangeWorld(world);
  });
  renderer_->AddAction(quit_action);
  renderer_->AddAction(move_square_action_up);
  renderer_->AddAction(move_square_action_down);
  renderer_->AddAction(move_square_action_left);
  renderer_->AddAction(move_square_action_right);
  renderer_->AddAction(square_fall_action);
  // world init settings
  WorldPtr world = std::make_shared<World>("play_world");
  VideoEntityPtr background = std::make_shared<VideoEntity>("background");
  background->SetSourceFile("resource/background.png");
  background->SetVisible(true);
  SetRenderVetexFromRect2D(background, 0.0f, 0.0f, kSdlWindowWidth,
                           kSdlWindowHeight, kSdlWindowWidth, kSdlWindowHeight);
  VideoEntityPtr square1 = std::make_shared<VideoEntity>("square1");
  square1->SetSourceFile("resource/square2.png");
  square1->SetVisible(false);
  world->AddObject(background);
  world->AddObject(square1);
  renderer_->AddWorld(world);
  renderer_->SetRenderWorld("play_world");
}

void TetrisRule::ChangeWorld(mofu::WorldPtr world) {
  if (!is_active_square_exist_) {
    PlaceActiveSquare(GetRandomActiveSquare());
  }
  auto objects = world->GetObjects();
  for (const auto &object : objects) {
    if (object.second->GetId() != "background") {
      object.second->SetRenderVertice({});
    }
  }
  for (int i = active_square_.shape.size() - 1; i >= 0; --i) {
    for (int j = 0; j < active_square_.shape[0].size(); ++j) {
      int x = active_square_.x + j;
      int y = active_square_.y + i;
      if (x >= 0 && x < chess_board_[0].size() && y >= 0 &&
          y < chess_board_.size()) {
        std::string object_name;
        switch (active_square_.shape[i][j]) {
        case kEmpty:
          break;
        case kType0:
        case kDead:
          object_name = "square1";
          break;
        default:
          break;
        }
        if (!object_name.empty()) {
          auto object = world->GetObject(object_name);
          if (object) {
            if (!object->GetVisible()) {
              object->SetVisible(true);
            }
            AddRenderVetexFromRect2D(object, x * square_width_,
                                     window_height_ - (y + 1) * square_height_,
                                     square_width_, square_height_,
                                     window_width_, window_height_);
          }
        }
      }
    }
  }
  for (int i = chess_board_.size() - 1; i >= 0; --i) {
    for (int j = 0; j < chess_board_[0].size(); ++j) {
      std::string object_name;
      switch (chess_board_[i][j]) {
      case kEmpty:
        break;
      case kType0:
      case kDead:
        object_name = "square1";
        break;
      default:
        break;
      }
      if (!object_name.empty()) {
        auto object = world->GetObject(object_name);
        if (object) {
          if (!object->GetVisible()) {
            object->SetVisible(true);
          }
          AddRenderVetexFromRect2D(object, j * square_width_,
                                   window_height_ - (i + 1) * square_height_,
                                   square_width_, square_height_, window_width_,
                                   window_height_);
        }
      }
    }
  }
  if (is_game_over_) {
    return;
  }
}

void Tetris::RunGame() {
  if (!renderer_->Init()) {
    LOGE("Run game failed!");
    return;
  }
  renderer_->Flush();
}

} // namespace mofu
