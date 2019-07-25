/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "template/tetris/tetris.h"
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

TetrisRule::Square GenerateRandomInitSquare() {}

TetrisRule::TetrisRule(int row_number, int col_number, float square_width,
                       float square_height)
    : chess_board_{}, row_number_(row_number), col_number_(col_number),
      square_width_(square_width),
      square_height_(square_height), active_square_{},
      is_active_square_exist_(false), score_(0), is_game_over_(false) {}

void TetrisRule::InitChessBoard() {
  chess_board_ = std::vector<std::vector<SquareType>>(
      row_number_, std::vector<SquareType>(col_number_, kEmpty));
}

void TetrisRule::PlaceActiveSquare(const Square &square) {}

void TetrisRule::SqaureDown() {
  if (is_game_over_) {
    return;
  }
  active_square_.y += 1;
  if (CheckTouchBottom()) {
    CheckGameOver();
    MakeActiveSquareDead();
    if (is_game_over_) {
      return;
    }
    score_ += CheckDeadSqaureWiped();
    is_active_square_exist_ = false;
  }
}

void TetrisRule::SqaureLeft() {}

void TetrisRule::SqaureRight() {}

void TetrisRule::SqaureRotate() {}

void TetrisRule::ResetChessBoard() {}

bool TetrisRule::CheckTouchBottom() {
  // check from the bottom of square
  for (int i = active_square_.shape.size() - 1; i >= 0; --i) {
    for (int j = 0; j < active_square_.shape[0].size(); ++j) {
      if (active_square_.shape[i][j] == kEmpty) {
        continue;
      }
      int x = active_square_.x + j;
      int y = active_square_.y + i;
      if (y == chess_board_.size() - 1 || chess_board_[y + 1][x] != kEmpty) {
        return true;
      }
    }
  }
  return false;
}

void TetrisRule::MakeActiveSquareDead() {
  for (int i = active_square_.shape.size() - 1; i >= 0; --i) {
    for (int j = 0; j < active_square_.shape[0].size(); ++j) {
      if (active_square_.shape[i][j] == kEmpty) {
        continue;
      }
      int x = active_square_.x + j;
      int y = active_square_.y + i;
      if (x >= 0 && x < chess_board_[0].size() && y >= 0 &&
          y < chess_board_.size()) {
        chess_board_[y][x] = kDead;
      }
    }
  }
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
      chess_board_[0] = std::vector<SquareType>(col_number_, kEmpty);
      ++wiped_line_number;
    }
  }
  return wiped_line_number;
}

void TetrisRule::CheckGameOver() { is_game_over_ = (active_square_.y < 0); }

Tetris::Tetris() : renderer_(nullptr), rule_(nullptr) {}

void Tetris::InitGame() {
  rule_ = std::make_shared<TetrisRule>(kSquareRowNumber, kSquareColNumber,
                                       kSdlWindowWidth / kSquareColNumber,
                                       kSdlWindowHeight / kSquareRowNumber);
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
    auto object = world->GetObject("square1");
    if (object) {
    }
  });
  ControlActionPtr move_square_action_down =
      std::make_shared<ControlAction>("down_action");
  move_square_action_down->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN;
  });
  move_square_action_down->SetActionCallback([=](WorldPtr world) {
    auto object = world->GetObject("square1");
    if (object) {
    }
  });
  ControlActionPtr move_square_action_left =
      std::make_shared<ControlAction>("left_action");
  move_square_action_left->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT;
  });
  move_square_action_left->SetActionCallback([=](WorldPtr world) {
    auto object = world->GetObject("square1");
    if (object) {
    }
  });
  ControlActionPtr move_square_action_right =
      std::make_shared<ControlAction>("right_action");
  move_square_action_right->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT;
  });
  move_square_action_right->SetActionCallback([=](WorldPtr world) {
    auto object = world->GetObject("square1");
    if (object) {
    }
  });
  GlobalTimerActionPtr square_fall_action =
      std::make_shared<GlobalTimerAction>("fall_action");
  TimerPtr square_fall_action_timer = std::make_shared<Timer>();
  square_fall_action_timer->SetTimerCallback(
      [=](std::chrono::time_point<std::chrono::system_clock> now) -> bool {
        bool ret = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now - square_fall_action_timer->GetStart())
                       .count() > 800;
        if (ret) {
          square_fall_action_timer->Start();
        }
        return ret;
      });
  square_fall_action->SetTimer(square_fall_action_timer);
  square_fall_action->SetActionCallback([=](WorldPtr world) {
    auto object = world->GetObject("square1");
    if (object) {
    }
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
  square1->SetSourceFile("resource/square1.png");
  square1->SetVisible(true);
  SetRenderVetexFromRect2D(square1, 0.0f,
                           kSdlWindowHeight - kSdlWindowHeight / 16.0f,
                           kSdlWindowWidth / 12.0f, kSdlWindowHeight / 16.0f,
                           kSdlWindowWidth, kSdlWindowHeight);
  VideoEntityPtr dead_square = std::make_shared<VideoEntity>("dead_square");
  dead_square->SetVisible(true);
  dead_square->SetSharedEntity(square1.get());
  world->AddObject(background);
  world->AddObject(square1);
  renderer_->AddWorld(world);
  renderer_->SetRenderWorld("play_world");
}

void TetrisRule::ChangeWorld(mofu::WorldPtr world) {
  if (is_game_over_) {
    return;
  }
  if (!is_active_square_exist_) {
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
