/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "template/tetris/tetris.h"

namespace {

constexpr char kGameName[] = "Tetris";
constexpr float kSdlWindowWidth = 480.0f;
constexpr float kSdlWindowHeight = 640.0f;

} // namespace

namespace mofu {

Tetris::Tetris() : renderer_(nullptr) {}

void Tetris::InitGame() {
  renderer_ =
      std::make_shared<Renderer>(kGameName, kSdlWindowWidth, kSdlWindowHeight);
  // action settings
  QuitActionPtr quit_action = std::make_shared<QuitAction>();
  quit_action->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE;
  });
  ControlActionPtr move_square_action_up = std::make_shared<ControlAction>();
  move_square_action_up->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP;
  });
  move_square_action_up->SetActionCallback([](WorldPtr world) {
    for (int i = 0; i < world->GetObjectsNumber(); ++i) {
      auto object = world->GetObject(i);
      if (object && object->GetId() == "square1") {
        std::vector<VideoEntity::Position> positinos;
        for (int j = 0; j < object->GetPositionsNumber(); ++j) {
          auto position = object->GetPosition(j);
          positinos.push_back(VideoEntity::Position{
              position.x,
              std::max(0.0f, position.y - position.height),
              position.width,
              position.height,
              position.size,
          });
        }
        object->SetPosition(positinos);
      }
    }
  });
  ControlActionPtr move_square_action_down = std::make_shared<ControlAction>();
  move_square_action_down->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN;
  });
  move_square_action_down->SetActionCallback([](WorldPtr world) {
    for (int i = 0; i < world->GetObjectsNumber(); ++i) {
      auto object = world->GetObject(i);
      if (object && object->GetId() == "square1") {
        std::vector<VideoEntity::Position> positinos;
        for (int j = 0; j < object->GetPositionsNumber(); ++j) {
          auto position = object->GetPosition(j);
          positinos.push_back(VideoEntity::Position{
              position.x,
              std::min(kSdlWindowHeight - position.height,
                       position.y + position.height),
              position.width,
              position.height,
              position.size,
          });
        }
        object->SetPosition(positinos);
      }
    }
  });
  ControlActionPtr move_square_action_left = std::make_shared<ControlAction>();
  move_square_action_left->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT;
  });
  move_square_action_left->SetActionCallback([](WorldPtr world) {
    for (int i = 0; i < world->GetObjectsNumber(); ++i) {
      auto object = world->GetObject(i);
      if (object && object->GetId() == "square1") {
        std::vector<VideoEntity::Position> positinos;
        for (int j = 0; j < object->GetPositionsNumber(); ++j) {
          auto position = object->GetPosition(j);
          positinos.push_back(VideoEntity::Position{
              std::max(0.0f, position.x - position.width),
              position.y,
              position.width,
              position.height,
              position.size,
          });
        }
        object->SetPosition(positinos);
      }
    }
  });
  ControlActionPtr move_square_action_right = std::make_shared<ControlAction>();
  move_square_action_right->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT;
  });
  move_square_action_right->SetActionCallback([](WorldPtr world) {
    for (int i = 0; i < world->GetObjectsNumber(); ++i) {
      auto object = world->GetObject(i);
      if (object && object->GetId() == "square1") {
        std::vector<VideoEntity::Position> positinos;
        for (int j = 0; j < object->GetPositionsNumber(); ++j) {
          auto position = object->GetPosition(j);
          positinos.push_back(VideoEntity::Position{
              std::min(kSdlWindowWidth - position.width,
                       position.x + position.width),
              position.y,
              position.width,
              position.height,
              position.size,
          });
        }
        object->SetPosition(positinos);
      }
    }
  });
  renderer_->AddAction(quit_action);
  renderer_->AddAction(move_square_action_up);
  renderer_->AddAction(move_square_action_down);
  renderer_->AddAction(move_square_action_left);
  renderer_->AddAction(move_square_action_right);
  // world init settings
  WorldPtr world = std::make_shared<World>();
  VideoEntityPtr background = std::make_shared<VideoEntity>("background");
  background->SetSourceFile("resource/background.png");
  background->AddPosition(VideoEntity::Position{
      0.0f,
      0.0f,
      kSdlWindowWidth,
      kSdlWindowHeight,
      1.0f,
  });
  VideoEntityPtr square1 = std::make_shared<VideoEntity>("square1");
  square1->SetSourceFile("resource/square1.png");
  square1->AddPosition(VideoEntity::Position{
      0.0f,
      0.0f,
      kSdlWindowWidth / 12.0f,
      kSdlWindowHeight / 16.0f,
      1.0f,
  });
  world->AddObject(background);
  world->AddObject(square1);
  renderer_->SetWorld(world);
}

void Tetris::RunGame() {
  renderer_->Init();
  renderer_->Flush();
}

} // namespace mofu
