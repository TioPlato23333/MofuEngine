/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "template/tetris/tetris.h"
#include "core/log.h"

namespace {

constexpr char kGameName[] = "Tetris";
constexpr float kSdlWindowWidth = 480.0f;
constexpr float kSdlWindowHeight = 640.0f;

} // namespace

namespace mofu {

Tetris::Tetris() : renderer_(nullptr) {}

void Tetris::InitGame() {
  // general function
  auto square_dead_and_renew = [=](WorldPtr world) {
    auto object = world->GetObject("square1");
    auto dead_object = world->GetObject("dead_square");
    if (object && dead_object) {
      bool touch_bottom = false;
      for (const auto &position : object->GetPositions()) {
        if (position.y >= kSdlWindowHeight - position.height) {
          touch_bottom = true;
          break;
        }
        for (const auto &dead_position : dead_object->GetPositions()) {
          if (position.x == dead_position.x &&
              position.y >= dead_position.y - position.height) {
            touch_bottom = true;
            break;
          }
        }
        if (touch_bottom) {
          break;
        }
      }
      if (touch_bottom) {
        for (const auto &position : object->GetPositions()) {
          dead_object->AddPosition(VideoEntity::Position{
              position.x,
              position.y,
              kSdlWindowWidth / 12.0f,
              kSdlWindowHeight / 16.0f,
              1.0f,
          });
        }
        bool chess_board[16][12] = {};
        for (const auto &dead_position : dead_object->GetPositions()) {
          int width =
              static_cast<int>(dead_position.x / (kSdlWindowWidth / 12.0f));
          int height =
              static_cast<int>(dead_position.y / (kSdlWindowHeight / 16.0f));
          chess_board[height][width] = true;
        }
        for (int i = 15; i >= 0; --i) {
          bool line_disappear = true;
          for (int j = 0; j < 12; ++j) {
            line_disappear = line_disappear && chess_board[i][j];
          }
          if (line_disappear) {
            for (int j = i; j >= 1; --j) {
              std::memcpy(chess_board[j], chess_board[j - 1],
                          12 * sizeof(bool));
            }
            memset(chess_board[0], 0, 12 * sizeof(bool));
            ++i;
          }
        }
        std::vector<VideoEntity::Position> positinos;
        for (int i = 0; i < 16; ++i) {
          for (int j = 0; j < 12; ++j) {
            if (chess_board[i][j]) {
              positinos.push_back(VideoEntity::Position{
                  j * kSdlWindowWidth / 12.0f,
                  i * kSdlWindowHeight / 16.0f,
                  kSdlWindowWidth / 12.0f,
                  kSdlWindowHeight / 16.0f,
                  1.0f,
              });
            }
          }
        }
        dead_object->SetPositions(positinos);
        object->SetPositions({VideoEntity::Position{
            0.0f,
            0.0f,
            kSdlWindowWidth / 12.0f,
            kSdlWindowHeight / 16.0f,
            1.0f,
        }});
      }
    }
  };
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
      std::vector<VideoEntity::Position> positinos;
      for (const auto &position : object->GetPositions()) {
        positinos.push_back(VideoEntity::Position{
            position.x,
            std::max(0.0f, position.y - position.height),
            position.width,
            position.height,
            position.size,
        });
      }
      object->SetPositions(positinos);
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
      std::vector<VideoEntity::Position> positinos;
      for (const auto &position : object->GetPositions()) {
        positinos.push_back(VideoEntity::Position{
            position.x,
            std::min(kSdlWindowHeight - position.height,
                     position.y + position.height),
            position.width,
            position.height,
            position.size,
        });
      }
      object->SetPositions(positinos);
      square_dead_and_renew(world);
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
      std::vector<VideoEntity::Position> positinos;
      for (const auto &position : object->GetPositions()) {
        positinos.push_back(VideoEntity::Position{
            std::max(0.0f, position.x - position.width),
            position.y,
            position.width,
            position.height,
            position.size,
        });
      }
      object->SetPositions(positinos);
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
      std::vector<VideoEntity::Position> positinos;
      for (const auto &position : object->GetPositions()) {
        positinos.push_back(VideoEntity::Position{
            std::min(kSdlWindowWidth - position.width,
                     position.x + position.width),
            position.y,
            position.width,
            position.height,
            position.size,
        });
      }
      object->SetPositions(positinos);
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
      std::vector<VideoEntity::Position> positinos;
      for (const auto &position : object->GetPositions()) {
        positinos.push_back(VideoEntity::Position{
            position.x,
            std::min(kSdlWindowHeight - position.height,
                     position.y + position.height),
            position.width,
            position.height,
            position.size,
        });
      }
      object->SetPositions(positinos);
      square_dead_and_renew(world);
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
  background->AddPosition(VideoEntity::Position{
      0.0f,
      0.0f,
      kSdlWindowWidth,
      kSdlWindowHeight,
      1.0f,
  });
  VideoEntityPtr square1 = std::make_shared<VideoEntity>("square1");
  square1->SetSourceFile("resource/square1.png");
  square1->SetVisible(true);
  square1->AddPosition(VideoEntity::Position{
      0.0f,
      0.0f,
      kSdlWindowWidth / 12.0f,
      kSdlWindowHeight / 16.0f,
      1.0f,
  });
  VideoEntityPtr dead_square = std::make_shared<VideoEntity>("dead_square");
  dead_square->SetVisible(true);
  dead_square->SetSharedEntity(square1.get());
  world->AddObject(background);
  world->AddObject(square1);
  world->AddObject(dead_square);
  renderer_->AddWorld(world);
  renderer_->SetRenderWorld("play_world");
}

void Tetris::RunGame() {
  if (!renderer_->Init()) {
    LOGE("Run game failed!");
    return;
  }
  renderer_->Flush();
}

} // namespace mofu
