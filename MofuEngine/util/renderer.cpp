/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "util/renderer.h"
#include "core/log.h"

namespace {

constexpr int kSdlWindowWidth = 480;
constexpr int kSdlWindowHeight = 640;

} // namespace

namespace mofu {

Action::Action() : callback_() {}

void Action::SetActionCallback(const std::function<void(WorldPtr)> &callback) {
  callback_ = callback;
}

std::function<void(WorldPtr)> Action::GetActionCallback() const {
  return callback_;
}

bool Renderer::Init() {
  SDL_Init(SDL_INIT_VIDEO);
  window_ = SDL_CreateWindow(window_name_.c_str(), SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kSdlWindowWidth,
                             kSdlWindowHeight, SDL_WINDOW_OPENGL);
  if (!window_) {
    LOGE("Sdl window init failed.");
    return false;
  }
  return true;
}

void Renderer::Flush() {
  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (control_checker_) {
        auto type = control_checker_(event);
        if (type == Action::kQuit) {
          Close();
          break;
        }
        auto action = GetAction(type);
        if (action) {
          auto callback = action->GetActionCallback();
          if (callback) {
            callback(world_);
          }
        }
      }
    }
  }
}

void Renderer::Close() {
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

Renderer::Renderer(std::string window_name)
    : window_name_(std::move(window_name)), window_(nullptr), world_(nullptr),
      video_decoder_(nullptr), actions_{}, control_checker_() {}

void Renderer::AddAction(Action::ActionType type, ActionPtr action) {
  actions_.insert(std::make_pair(type, std::move(action)));
}

void Renderer::SetActions(
    const std::map<Action::ActionType, ActionPtr> &actions) {
  actions_ = actions;
}

void Renderer::SetControlChecker(
    const std::function<Action::ActionType(SDL_Event)> &checker) {
  control_checker_ = checker;
}

ActionPtr Renderer::GetAction(Action::ActionType type) const {
  auto it = actions_.find(type);
  if (it == actions_.end()) {
    return nullptr;
  }
  return it->second;
}

std::function<Action::ActionType(SDL_Event)>
Renderer::GetControlChecker() const {
  return control_checker_;
}

} // namespace mofu
