/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef UTIL_RENDERER_H_
#define UTIL_RENDERER_H_

#include <SDL.h>
#include <functional>
#include <map>

#include "core/core.h"
#include "video/video.h"

namespace mofu {

class Action {
public:
  enum ActionType : uint8_t {
    kDoNothing,
    kQuit,
  };

public:
  Action();
  ~Action() = default;

  void SetActionCallback(const std::function<void(WorldPtr)> &callback);
  std::function<void(WorldPtr)> GetActionCallback() const;

private:
  std::function<void(WorldPtr)> callback_;
};

using ActionPtr = std::shared_ptr<Action>;

class Renderer {
public:
  explicit Renderer(std::string window_name);
  ~Renderer() = default;

  bool Init();
  void Flush();
  void Close();
  void AddAction(Action::ActionType type, ActionPtr action);
  void SetActions(const std::map<Action::ActionType, ActionPtr> &actions);
  void SetControlChecker(
      const std::function<Action::ActionType(SDL_Event)> &checker);
  ActionPtr GetAction(Action::ActionType type) const;
  std::function<Action::ActionType(SDL_Event)> GetControlChecker() const;

private:
  std::string window_name_;
  SDL_Window *window_;
  WorldPtr world_;
  VideoDecoderPtr video_decoder_;
  std::map<Action::ActionType, ActionPtr> actions_;
  std::function<Action::ActionType(SDL_Event)> control_checker_;
};

using RendererPtr = std::shared_ptr<Renderer>;

} // namespace mofu

#endif // UTIL_RENDERER_H_
