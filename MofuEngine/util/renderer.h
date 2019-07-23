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
  ControlAction();
  ~ControlAction() override = default;

  void SetControlChecker(const std::function<bool(SDL_Event)> &checker);
  std::function<bool(SDL_Event)> GetControlChecker() const;

protected:
  std::function<bool(SDL_Event)> control_checker_;
};

class TimerAction : public Action {
public:
  TimerAction();
  ~TimerAction() override = default;

  void SetTimer(TimerPtr timer);
  TimerPtr GetTimer() const;

protected:
  TimerPtr timer_;
};

class GlobalTimerAction : public TimerAction {
public:
  GlobalTimerAction() = default;
  ~GlobalTimerAction() override = default;
};

class QuitAction : public ControlAction {
public:
  QuitAction() = default;
  ~QuitAction() override = default;
};

using ControlActionPtr = std::shared_ptr<ControlAction>;
using GlobalTimerActionPtr = std::shared_ptr<GlobalTimerAction>;
using QuitActionPtr = std::shared_ptr<QuitAction>;

class Renderer {
public:
  explicit Renderer(std::string window_name);
  ~Renderer() = default;

  bool Init();
  void Flush();
  void Close();
  void AddAction(ActionPtr action);
  void SetActions(const std::vector<ActionPtr> &actions);
  void SetWorld(WorldPtr world);
  ActionPtr GetAction(int i) const;
  int GetActionsNumber();
  WorldPtr GetWorld() const;

private:
  void DrawWorld();

  std::string window_name_;
  SDL_Window *window_;
  WorldPtr world_;
  VideoDecoderPtr video_decoder_;
  std::vector<ActionPtr> actions_;
  ShaderProgramPoolPtr pool_;
  SDL_GLContext gl_ctx_;
};

using RendererPtr = std::shared_ptr<Renderer>;

} // namespace mofu

#endif // UTIL_RENDERER_H_
