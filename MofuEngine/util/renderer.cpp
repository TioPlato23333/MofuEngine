/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#include "util/renderer.h"

#include <thread>

#include "core/log.h"
#include "util/util.h"

namespace {

constexpr int kMaxFrameRate = 30;

} // namespace

namespace mofu {

ControlAction::ControlAction(const std::string &id)
    : Action(id), control_checker_() {}

void ControlAction::SetControlChecker(
    const std::function<bool(SDL_Event)> &checker) {
  control_checker_ = checker;
}

std::function<bool(SDL_Event)> ControlAction::GetControlChecker() const {
  return control_checker_;
}

TimerAction::TimerAction(const std::string &id) : Action(id), timer_(nullptr) {}

GlobalTimerAction::GlobalTimerAction(const std::string &id) : TimerAction(id) {}

QuitAction::QuitAction(const std::string &id) : ControlAction(id) {}

void TimerAction::SetTimer(mofu::TimerPtr timer) { timer_ = std::move(timer); }

TimerPtr TimerAction::GetTimer() const { return timer_; }

bool Renderer::Init() {
  SDL_Init(SDL_INIT_VIDEO);
  window_ = SDL_CreateWindow(window_name_.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, window_width_,
                             window_height_, SDL_WINDOW_OPENGL);
  if (!window_) {
    LOGE("Sdl window init failed.");
    return false;
  }
  gl_ctx_ = SDL_GL_CreateContext(window_);
  LOGD("Current GL_VERSION is %s.", glGetString(GL_VERSION));
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
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
  pool_ = std::make_shared<ShaderProgramPool>();
  if (!render_world_) {
    LOGE("No render world.");
    return false;
  }
  return true;
}

void Renderer::Flush() {
  DrawWorld();
  // start global timer action timing
  for (const auto &it : actions_) {
    auto action = it.second;
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
      for (const auto &it : actions_) {
        auto action = it.second;
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
              callback(rendering_world_);
            }
          }
        }
      }
    }
    // check timer action
    for (const auto &it : actions_) {
      auto action = it.second;
      auto timer_action_ptr = std::dynamic_pointer_cast<TimerAction>(action);
      if (timer_action_ptr) {
        auto timer = timer_action_ptr->GetTimer();
        if (timer) {
          auto timer_callback = timer->GetTimerCallback();
          if (timer_callback &&
              timer_callback(std::chrono::system_clock::now())) {
            auto callback = timer_action_ptr->GetActionCallback();
            if (callback) {
              callback(rendering_world_);
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

Renderer::~Renderer() { Close(); }

void Renderer::Close() {
  DestroyWorld();
  pool_.reset();
  if (gl_ctx_) {
    SDL_GL_DeleteContext(gl_ctx_);
  }
  if (window_) {
    SDL_DestroyWindow(window_);
  }
  SDL_Quit();
}

Renderer::Renderer(std::string window_name, int window_width, int window_height)
    : window_name_(std::move(window_name)), window_(nullptr), worlds_{},
      video_decoder_(nullptr), actions_{}, pool_(nullptr), gl_ctx_(nullptr),
      window_width_(window_width), window_height_(window_height),
      render_world_(nullptr), rendering_world_(nullptr) {}

void Renderer::AddAction(ActionPtr action) {
  actions_.emplace(std::make_pair(action->GetId(), action));
}

void Renderer::SetActions(const std::map<std::string, ActionPtr> &actions) {
  actions_ = actions;
}

ActionPtr Renderer::GetAction(const std::string &id) const {
  auto it = actions_.find(id);
  if (it == actions_.end()) {
    LOGE("Renderer action not found.");
    return nullptr;
  }
  return it->second;
}

std::map<std::string, ActionPtr> const &Renderer::GetActions() const {
  return actions_;
}

bool Renderer::LoadWorld() {
  if (!rendering_world_) {
    LOGE("Rendering world is null when loading.");
    return false;
  }
  for (const auto &it : rendering_world_->GetObjects()) {
    auto object = it.second;
    if (object && !object->GetSourceFile().empty()) {
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
      GLuint tex = CreateTexFromPixelData(video_decoder_->GetData(),
                                          video_decoder_->GetWidth(),
                                          video_decoder_->GetHeight());
      object->SetResourceId(tex);
      video_decoder_->Close();
    }
  }
  return true;
}

void Renderer::DestroyWorld() {
  if (!rendering_world_) {
    return;
  }
  for (const auto &it : rendering_world_->GetObjects()) {
    auto object = it.second;
    if (object && object->GetResourceId() != 0) {
      GLuint tex = object->GetResourceId();
      glDeleteTextures(1, &tex);
    }
  }
}

void Renderer::DrawWorld() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  CheckGlError("glClear");

  if (render_world_ != rendering_world_ ||
      render_world_->GetId() != rendering_world_->GetId()) {
    DestroyWorld();
    rendering_world_ = render_world_;
    if (!LoadWorld()) {
      return;
    }
  }
  for (const auto &it : rendering_world_->GetObjects()) {
    auto object = it.second;
    if (object && object->GetVisible()) {
      GLuint resource_id;
      if (object->GetSharedEntity()) {
        resource_id = object->GetSharedEntity()->GetResourceId();
      } else {
        resource_id = object->GetResourceId();
      }
      if (resource_id == 0) {
        continue;
      }
      std::vector<GLfloat> vetex_coord;
      int vertice_count = 0;
      for (const auto &vertex : object->GetRenderVertice()) {
        vetex_coord.emplace_back(std::get<0>(vertex));
        vetex_coord.emplace_back(std::get<1>(vertex));
        vetex_coord.emplace_back(std::get<2>(vertex));
        ++vertice_count;
        if (vertice_count % 4 == 0) { // currently only support 2D texture
          pool_->GetDrawEntityShaderProgram()->Run(resource_id,
                                                   vetex_coord.data(), nullptr);
          vetex_coord = {};
        }
      }
    }
  }
  SDL_GL_SwapWindow(window_);
}

void Renderer::AddWorld(mofu::WorldPtr world) {
  worlds_.emplace(std::make_pair(world->GetId(), world));
}

void Renderer::SetWorlds(const std::map<std::string, WorldPtr> &worlds) {
  worlds_ = worlds;
}

WorldPtr Renderer::GetWorld(const std::string &id) const {
  auto it = worlds_.find(id);
  if (it == worlds_.end()) {
    LOGE("Renderer world not found.");
    return nullptr;
  }
  return it->second;
}

std::map<std::string, WorldPtr> const &Renderer::GetWorlds() const {
  return worlds_;
}

int Renderer::GetWindowWidth() const { return window_width_; }

int Renderer::GetWindowHeight() const { return window_height_; }

void Renderer::SetRenderWorld(const std::string &id) {
  auto it = worlds_.find(id);
  if (it == worlds_.end()) {
    LOGE("Renderer world not found when trying to change world.");
    return;
  }
  render_world_ = it->second;
}

} // namespace mofu
