/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "template/tetris/tetris.h"

namespace {

constexpr char kGameName[] = "Tetris";

} // namespace

namespace mofu {

Tetris::Tetris() : renderer_(nullptr) {}

void Tetris::InitGame() {
  renderer_ = std::make_shared<Renderer>(kGameName);
  // action settings
  QuitActionPtr quit_action = std::make_shared<QuitAction>();
  quit_action->SetControlChecker([](SDL_Event event) -> bool {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE;
  });
  renderer_->AddAction(quit_action);
  // world init settings
  WorldPtr world = std::make_shared<World>();
  VideoEntityPtr background = std::make_shared<VideoEntity>();
  background->SetSourceFile("resource/background.png");
  world->AddObject(background);
  renderer_->SetWorld(world);
}

void Tetris::RunGame() {
  renderer_->Init();
  renderer_->Flush();
}

} // namespace mofu
