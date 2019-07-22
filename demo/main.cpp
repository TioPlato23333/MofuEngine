/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "core/log.h"
#include "util/renderer.h"

int main(int argc, char *argv[]) {
  mofu::RendererPtr renderer = std::make_shared<mofu::Renderer>("demo");
  renderer->SetControlChecker([](SDL_Event event) -> mofu::Action::ActionType {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
      return mofu::Action::kQuit;
    } else {
      return mofu::Action::kDoNothing;
    }
  });
  renderer->Init();
  renderer->Flush();
  return 0;
}
