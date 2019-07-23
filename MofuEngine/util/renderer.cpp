/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "util/renderer.h"

#include <thread>

#include "core/log.h"

namespace {

constexpr int kSdlWindowWidth = 480;
constexpr int kSdlWindowHeight = 640;
constexpr int kMaxFrameRate = 30;

} // namespace

namespace mofu {

ControlAction::ControlAction() : control_checker_() {}

void ControlAction::SetControlChecker(
    const std::function<bool(SDL_Event)> &checker) {
  control_checker_ = checker;
}

std::function<bool(SDL_Event)> ControlAction::GetControlChecker() const {
  return control_checker_;
}

TimerAction::TimerAction() : timer_(nullptr) {}

void TimerAction::SetTimer(mofu::TimerPtr timer) { timer_ = std::move(timer); }

TimerPtr TimerAction::GetTimer() const { return timer_; }

bool Renderer::Init() {
  SDL_Init(SDL_INIT_VIDEO);
  window_ = SDL_CreateWindow(window_name_.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, kSdlWindowWidth,
                             kSdlWindowHeight, SDL_WINDOW_OPENGL);
  if (!window_) {
    LOGE("Sdl window init failed.");
    return false;
  }
  gl_ctx_ = SDL_GL_CreateContext(window_);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  if (glewInit() != GLEW_OK) {
    LOGE("Glew init failed.");
    return false;
  }
  av_register_all();
  video_decoder_ = std::make_shared<VideoDecoder>();
  for (int i = 0; i < world_->GetObjectsNumber(); ++i) {
    auto object = world_->GetObject(i);
    if (object) {
      if (!video_decoder_->Open(object->GetSourceFile())) {
        LOGE("Video entity source open failed.");
        video_decoder_->Close();
        return false;
      }
      ErrorInfo error;
      video_decoder_->ReadFrame(&error);
      if (error.GetType() != ErrorInfo::kOk &&
          error.GetType() != ErrorInfo::kEof) {
        LOGE("Video entity source decode failed.");
        video_decoder_->Close();
        return false;
      }
      GLuint tex = gl::CreateTexFromPixelData(video_decoder_->GetData(),
                                              video_decoder_->GetWidth(),
                                              video_decoder_->GetHeight());
      object->SetResourceId(tex);
      video_decoder_->Close();
    }
  }
  pool_ = std::make_shared<ShaderProgramPool>();
  return true;
}

void Renderer::Flush() {
  DrawWorld();
  // start global timer action timing
  for (const auto &action : actions_) {
    auto global_timer_action_ptr =
        std::dynamic_pointer_cast<GlobalTimerAction>(action);
    if (global_timer_action_ptr) {
      auto timer = global_timer_action_ptr->GetTimer();
      if (timer) {
        timer->Start();
      }
    }
  }
  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      // check control action and quit action
      for (const auto &action : actions_) {
        auto control_action_ptr =
            std::dynamic_pointer_cast<ControlAction>(action);
        if (control_action_ptr) {
          auto quit_action_ptr =
              std::dynamic_pointer_cast<QuitAction>(control_action_ptr);
          if (quit_action_ptr) {
            auto checker = quit_action_ptr->GetControlChecker();
            if (checker && checker(event)) {
              Close();
              return;
            }
          }
          auto checker = control_action_ptr->GetControlChecker();
          if (checker && checker(event)) {
            auto callback = control_action_ptr->GetActionCallback();
            if (callback) {
              callback(world_);
            }
          }
        }
      }
    }
    // check timer action
    for (const auto &action : actions_) {
      auto timer_action_ptr = std::dynamic_pointer_cast<TimerAction>(action);
      if (timer_action_ptr) {
        auto timer = timer_action_ptr->GetTimer();
        if (timer) {
          auto timer_callback = timer->GetTimerCallback();
          if (timer_callback &&
              timer_callback(std::chrono::system_clock::now())) {
            auto callback = timer_action_ptr->GetActionCallback();
            if (callback) {
              callback(world_);
            }
          }
        }
      }
    }
    DrawWorld();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 / kMaxFrameRate));
  }
}

void Renderer::Close() {
  pool_.reset();
  SDL_GL_DeleteContext(gl_ctx_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

Renderer::Renderer(std::string window_name)
    : window_name_(std::move(window_name)), window_(nullptr), world_(nullptr),
      video_decoder_(nullptr), actions_{}, pool_(nullptr), gl_ctx_(nullptr) {}

void Renderer::AddAction(ActionPtr action) {
  actions_.push_back(std::move(action));
}

void Renderer::SetActions(const std::vector<ActionPtr> &actions) {
  actions_ = actions;
}

ActionPtr Renderer::GetAction(int i) const {
  if (actions_.empty() || i < 0 || i >= actions_.size()) {
    LOGE("World object out of index.");
    return nullptr;
  }
  return actions_[i];
}

int Renderer::GetActionsNumber() { return actions_.size(); }

void Renderer::DrawWorld() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  gl::CheckGlError("glClear");

  for (int i = 0; i < world_->GetObjectsNumber(); ++i) {
    auto object = world_->GetObject(i);
    if (object && object->GetResourceId() != 0) {
      pool_->GetDrawEntityShaderProgram()->Run(object->GetResourceId());
    }
  }
  SDL_GL_SwapWindow(window_);
}

void Renderer::SetWorld(mofu::WorldPtr world) { world_ = std::move(world); }

WorldPtr Renderer::GetWorld() const { return world_; }

} // namespace mofu
