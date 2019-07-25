/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef UTIL_RENDERER_H_
#define UTIL_RENDERER_H_

#include <SDL.h>

#include "core/core.h"
#include "util/shader_program.h"
#include "video/video.h"

namespace mofu {

class ControlAction : public Action {
public:
  explicit ControlAction(const std::string &id);
  ~ControlAction() override = default;

  void SetControlChecker(const std::function<bool(SDL_Event)> &checker);
  std::function<bool(SDL_Event)> GetControlChecker() const;

protected:
  std::function<bool(SDL_Event)> control_checker_;
};

class TimerAction : public Action {
public:
  explicit TimerAction(const std::string &id);
  ~TimerAction() override = default;

  void SetTimer(TimerPtr timer);
  TimerPtr GetTimer() const;

protected:
  TimerPtr timer_;
};

class GlobalTimerAction : public TimerAction {
public:
  explicit GlobalTimerAction(const std::string &id);
  ~GlobalTimerAction() override = default;
};

class QuitAction : public ControlAction {
public:
  explicit QuitAction(const std::string &id);
  ~QuitAction() override = default;
};

using ControlActionPtr = std::shared_ptr<ControlAction>;
using GlobalTimerActionPtr = std::shared_ptr<GlobalTimerAction>;
using QuitActionPtr = std::shared_ptr<QuitAction>;

class Renderer {
public:
  Renderer(std::string window_name, int window_width, int window_height);
  ~Renderer();

  bool Init();
  void Flush();
  void Close();
  void AddAction(ActionPtr action);
  void SetActions(const std::map<std::string, ActionPtr> &actions);
  void AddWorld(WorldPtr world);
  void SetWorlds(const std::map<std::string, WorldPtr> &worlds);
  ActionPtr GetAction(const std::string &id) const;
  std::map<std::string, ActionPtr> const &GetActions() const;
  int GetWindowWidth() const;
  int GetWindowHeight() const;
  WorldPtr GetWorld(const std::string &id) const;
  std::map<std::string, WorldPtr> const &GetWorlds() const;
  void SetRenderWorld(const std::string &id);

private:
  bool LoadWorld();
  void DestroyWorld();
  void DrawWorld();

  std::string window_name_;
  SDL_Window *window_;
  std::map<std::string, WorldPtr> worlds_;
  VideoDecoderPtr video_decoder_;
  std::map<std::string, ActionPtr> actions_;
  ShaderProgramPoolPtr pool_;
  SDL_GLContext gl_ctx_;
  int window_width_;
  int window_height_;
  WorldPtr render_world_;
  WorldPtr rendering_world_;
};

using RendererPtr = std::shared_ptr<Renderer>;

} // namespace mofu

#endif // UTIL_RENDERER_H_
