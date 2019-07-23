/* Created by Xiaodong Ning (ningxiaodong23333@gmail.com)
 *
 * Copyright (c) 2019 Xiaodong Ning. All rights reserved.
 */

#ifndef CORE_CORE_H_
#define CORE_CORE_H_

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace mofu {

class Entity {
public:
  explicit Entity(std::string id);
  virtual ~Entity() = default;

  void SetSourceFile(const std::string &source_file);
  std::string GetSourceFile() const;
  std::string GetId() const;

protected:
  std::string id_;
  std::string source_file_;
};

class VideoEntity : public Entity {
public:
  enum Depth : uint8_t {
    kLayout0,
    kLayout1,
    kLayout2,
    KLayoutMax,
  };

  struct Position {
    float x;
    float y;
    float width;
    float height;
    float size;
  };

public:
  explicit VideoEntity(std::string id);
  ~VideoEntity() override = default;

  void SetDepth(Depth depth);
  void SetVisible(bool visible);
  void AddPosition(const Position &position);
  void SetPosition(const std::vector<Position> &positions);
  void SetResourceId(int64_t resource_id);
  Depth GetDepth() const;
  bool GetVisible() const;
  Position GetPosition(int i) const;
  int64_t GetResourceId() const;
  int GetPositionsNumber();

private:
  Depth depth_;
  bool visible_;
  std::vector<Position> positions_;
  int64_t resource_id_;
  float size_;
};

class AudioEntity : public Entity {
public:
  explicit AudioEntity(std::string id);
  ~AudioEntity() override = default;
};

using VideoEntityPtr = std::shared_ptr<VideoEntity>;
using AudioEntityPtr = std::shared_ptr<AudioEntity>;

class World {
public:
  World();
  ~World() = default;

  void AddObject(VideoEntityPtr object);
  void SetObjects(const std::vector<VideoEntityPtr> &objects);
  void SetMusic(AudioEntityPtr music);
  VideoEntityPtr GetObject(int i) const;
  AudioEntityPtr GetMusic() const;
  int GetObjectsNumber();

private:
  std::vector<VideoEntityPtr> objects_;
  AudioEntityPtr music_;
};

using WorldPtr = std::shared_ptr<World>;

class Action {
public:
  Action();
  virtual ~Action() = default;

  void SetActionCallback(const std::function<void(WorldPtr)> &callback);
  std::function<void(WorldPtr)> GetActionCallback() const;

protected:
  std::function<void(WorldPtr)> callback_;
};

class Timer {
public:
  Timer();
  ~Timer() = default;

  void Start();
  void SetTimerCallback(
      const std::function<
          bool(std::chrono::time_point<std::chrono::system_clock>)> &callback);
  std::function<bool(std::chrono::time_point<std::chrono::system_clock>)>
  GetTimerCallback() const;
  std::chrono::time_point<std::chrono::system_clock> GetStart() const;

protected:
  std::chrono::time_point<std::chrono::system_clock> start_;
  std::function<bool(std::chrono::time_point<std::chrono::system_clock>)>
      callback_;
};

using ActionPtr = std::shared_ptr<Action>;
using TimerPtr = std::shared_ptr<Timer>;

} // namespace mofu

#endif // CORE_CORE_H_
